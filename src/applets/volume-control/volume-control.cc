#include "volume-control.hh"
#include "../../log.hh"
#include "icon-cache.hh"
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

auto double_fork() -> int {
	pid_t fork_id;
	if ((fork_id = fork()) == 0) {
		setsid();

		signal(SIGCHLD, SIG_IGN);
		signal(SIGHUP, SIG_IGN);

		if (fork() == 0) {
			umask(0);

			int x;
			for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
				close(x);
			}

			return 0;
		} else {
			exit(0);
		}
	} else {
		waitpid(fork_id, 0, 0);
		return 1;
	}
}

namespace wapanel::applet {

volume_control::volume_control(wap_t_applet_config applet_config, backend *backend)
	: m_backend(backend)
	, m_pop_control(GTK_MENU_BUTTON(gtk_menu_button_new()))
	, m_control_container(GTK_POPOVER(gtk_popover_new(GTK_WIDGET(m_pop_control))))
	, m_vol_widget_list(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8)))
	, m_output_widget(new volume_widget(backend, true))
	, m_input_widget(new volume_widget(backend, false)) {

	log_info("Created volume-control instance");

	// Config resolving

	if (wapi_key_exists(&applet_config.root, "__panel_height")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "__panel_height");
		m_config.__panel_height = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "flat")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "flat");

		if (var->type == WAP_CONF_VAR_TYPE_BOOLEAN) { m_config.is_flat = wapi_var_as_boolean(var); }
	}

	if (wapi_key_exists(&applet_config.root, "icon_height")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "icon_height");
		m_config.icon_height = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "sound_mixer")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "sound_mixer");

		if (var->type == WAP_CONF_VAR_TYPE_STRING) {
			m_config.sound_mixer_executable = std::string(wapi_var_as_string(var));
		} else {
			m_config.sound_mixer_executable = "";
		}
	}

	log_info("Resolved config");

	// Rest of config stuff

	if (m_config.is_flat) gtk_button_set_relief(GTK_BUTTON(m_pop_control), GTK_RELIEF_NONE);

	unsigned int icon_size = m_config.__panel_height * 1.5;
	if (m_config.icon_height != -1) {
		icon_size = m_config.icon_height;
	} else {
		m_config.icon_height = icon_size;
	}

	// GTK

	m_button_icon
		= GTK_IMAGE(gtk_image_new_from_pixbuf(ic::get_icon("audio-volume-muted-symbolic", m_config.icon_height)));

	gtk_container_add(GTK_CONTAINER(this->m_pop_control), GTK_WIDGET(this->m_button_icon));

	if (m_config.sound_mixer_executable != "") {
		m_open_sound_mixer = GTK_BUTTON(gtk_button_new_with_label("Sound mixer..."));
		gtk_button_set_relief(GTK_BUTTON(m_open_sound_mixer), GTK_RELIEF_NONE);

		g_signal_connect(m_open_sound_mixer, "clicked", G_CALLBACK(+[](GtkButton *button, std::string *exec_name) {
							 if (double_fork() == 0) {
								 system(exec_name->c_str());
								 exit(0);
							 }
						 }),
						 &m_config.sound_mixer_executable);

		gtk_box_pack_end(m_vol_widget_list, GTK_WIDGET(m_open_sound_mixer), false, false, 0);
	}

	gtk_box_pack_end(m_vol_widget_list, m_input_widget->get_widget(), false, false, 0);
	gtk_box_pack_end(m_vol_widget_list, m_output_widget->get_widget(), false, false, 0);

	gtk_container_add(GTK_CONTAINER(m_control_container), GTK_WIDGET(m_vol_widget_list));
	gtk_widget_show_all(GTK_WIDGET(m_vol_widget_list));
	gtk_widget_set_size_request(GTK_WIDGET(m_control_container), 370, -1);

	gtk_widget_set_margin_bottom(GTK_WIDGET(m_vol_widget_list), 6);
	gtk_widget_set_margin_top(GTK_WIDGET(m_vol_widget_list), 6);
	gtk_widget_set_margin_start(GTK_WIDGET(m_vol_widget_list), 6);
	gtk_widget_set_margin_end(GTK_WIDGET(m_vol_widget_list), 6);

	gtk_menu_button_set_popover(m_pop_control, GTK_WIDGET(m_control_container));

	// Change icon on volume change

	auto fn_icon_change = [=, this](float volume) {
		if (volume == 0) {
			gtk_image_set_from_pixbuf(m_button_icon, ic::get_icon("audio-volume-muted-symbolic", m_config.icon_height));
		} else if (volume <= 33) {
			gtk_image_set_from_pixbuf(m_button_icon, ic::get_icon("audio-volume-low-symbolic", m_config.icon_height));
		} else if (volume <= 66 && volume > 33) {
			gtk_image_set_from_pixbuf(m_button_icon,
									  ic::get_icon("audio-volume-medium-symbolic", m_config.icon_height));
		} else if (volume > 66) {
			gtk_image_set_from_pixbuf(m_button_icon, ic::get_icon("audio-volume-high-symbolic", m_config.icon_height));
		}
	};
	fn_icon_change(m_backend->get_output_volume_in_percent());

	m_backend->callback_output_volume_changed(fn_icon_change);
}
volume_control::~volume_control() {
	delete m_input_widget;
	delete m_output_widget;
}

auto volume_control::get_widget() -> GtkWidget * { return GTK_WIDGET(this->m_pop_control); }

// Volume widget

volume_widget::volume_widget(backend *backend, bool type_volume)
	: m_backend(backend)
	, m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6)))
	, m_volume_adj(GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 150, 1, 5, 0)))
	, m_volume_scale(GTK_SCALE(gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, m_volume_adj)))
	, m_mute_switch(GTK_SWITCH(gtk_switch_new())) {

	is_icon_change_locked = false;
	is_switched_outside = false;

	state_set_data = new _state_set_data();
	state_set_data->v1 = &is_switched_outside;
	state_set_data->v2 = m_backend;

	if (type_volume) {
		m_ind_icon = GTK_IMAGE(gtk_image_new_from_pixbuf(ic::get_icon("audio-volume-high-symbolic", 18)));

		auto fn_vol_change = [=, this](float volume) {
			if (!is_icon_change_locked) {
				is_icon_change_locked = true;

				if (volume == 0) {
					if (volume_state != vol_state::muted) {
						gtk_image_set_from_pixbuf(m_ind_icon, ic::get_icon("audio-volume-muted-symbolic", 18));
						volume_state = vol_state::muted;
					}
				} else if (volume <= 33) {
					if (volume_state != vol_state::low) {
						gtk_image_set_from_pixbuf(m_ind_icon, ic::get_icon("audio-volume-low-symbolic", 18));
						volume_state = vol_state::low;
					}
				} else if (volume <= 66 && volume > 33) {
					if (volume_state != vol_state::medium) {
						gtk_image_set_from_pixbuf(m_ind_icon, ic::get_icon("audio-volume-medium-symbolic", 18));
						volume_state = vol_state::medium;
					}
				} else if (volume > 66) {
					if (volume_state != vol_state::high) {
						gtk_image_set_from_pixbuf(m_ind_icon, ic::get_icon("audio-volume-high-symbolic", 18));
						volume_state = vol_state::high;
					}
				}

				is_icon_change_locked = false;
			}
		};
		fn_vol_change(m_backend->get_output_volume_in_percent());
		gtk_adjustment_set_value(m_volume_adj, m_backend->get_output_volume_in_percent());

		m_backend->callback_output_volume_changed(fn_vol_change);

		g_signal_connect(m_volume_adj, "value-changed",
						 G_CALLBACK(+[](GtkAdjustment *adjustment, wapanel::applet::backend *backend) {
							 backend->set_output_volume_in_percent(gtk_adjustment_get_value(adjustment));
							 return true;
						 }),
						 m_backend);

		gtk_switch_set_active(m_mute_switch, !m_backend->is_output_muted());
		g_signal_connect(m_mute_switch, "state-set",
						 G_CALLBACK(+[](GtkSwitch *gswitch, bool state, _state_set_data *data) {
							 if (*data->v1) {
								 *data->v1 = false;
							 } else {
								 if (state) {
									 data->v2->unmute_output();
								 } else {
									 data->v2->mute_output();
								 }
							 }
							 return false;
						 }),
						 state_set_data);
		m_backend->callback_output_mute_changed([=, this](bool state) {
			is_switched_outside = true;
			gtk_switch_set_active(m_mute_switch, !state);
			is_switched_outside = false;
		});
	} else {
		m_ind_icon = GTK_IMAGE(gtk_image_new_from_pixbuf(ic::get_icon("microphone-sensitivity-high-symbolic", 18)));

		auto fn_vol_change = [=, this](float volume) {
			if (!is_icon_change_locked) {
				is_icon_change_locked = true;

				if (volume <= 66) {
					if (volume_state != vol_state::muted) {
						gtk_image_set_from_pixbuf(m_ind_icon, ic::get_icon("microphone-sensitivity-low-symbolic", 18));
						volume_state = vol_state::muted;
					}
				} else if (volume > 66) {
					if (volume_state != vol_state::high) {
						gtk_image_set_from_pixbuf(m_ind_icon, ic::get_icon("microphone-sensitivity-high-symbolic", 18));
						volume_state = vol_state::high;
					}
				}

				is_icon_change_locked = false;
			}
		};
		fn_vol_change(m_backend->get_input_volume_in_percent());
		gtk_adjustment_set_value(m_volume_adj, m_backend->get_input_volume_in_percent());

		// Fast changing of image using pixbuf
		// can result in app crash and sometimes
		// compositor crash. Icons must be checked in
		// atomic way to stay thread-safe.
		// With two states it happens very rarely
		// investigate this in future.
		m_backend->callback_input_volume_changed(fn_vol_change);

		g_signal_connect(m_volume_adj, "value-changed",
						 G_CALLBACK(+[](GtkAdjustment *adjustment, wapanel::applet::backend *backend) {
							 backend->set_input_volume_in_percent(gtk_adjustment_get_value(adjustment));
							 return true;
						 }),
						 m_backend);

		gtk_switch_set_active(m_mute_switch, !m_backend->is_input_muted());
		g_signal_connect(m_mute_switch, "state-set",
						 G_CALLBACK(+[](GtkSwitch *gswitch, bool state, _state_set_data *data) {
							 if (*data->v1) {
								 *data->v1 = false;
							 } else {
								 if (state) {
									 data->v2->unmute_input();
								 } else {
									 data->v2->mute_input();
								 }
							 }
							 return false;
						 }),
						 state_set_data);
		m_backend->callback_input_mute_changed([=, this](bool state) {
			is_switched_outside = true;
			gtk_switch_set_active(m_mute_switch, !state);
			is_switched_outside = false;
		});
	}

	gtk_scale_set_draw_value(m_volume_scale, false);
	gtk_scale_add_mark(m_volume_scale, 100, GTK_POS_BOTTOM, NULL);

	gtk_widget_set_valign(GTK_WIDGET(m_mute_switch), GTK_ALIGN_CENTER);

	gtk_box_pack_end(m_root, GTK_WIDGET(m_mute_switch), false, false, 0);
	gtk_box_pack_end(m_root, GTK_WIDGET(m_volume_scale), true, true, 0);
	gtk_box_pack_end(m_root, GTK_WIDGET(m_ind_icon), false, false, 0);

	log_info("Created volume widget");
}
volume_widget::~volume_widget() {
	if(state_set_data != NULL)
		delete state_set_data;
}

auto volume_widget::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }

}