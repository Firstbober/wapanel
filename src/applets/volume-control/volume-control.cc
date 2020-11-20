#include "volume-control.hh"
#include "../../log.hh"
#include "icon-cache.hh"

namespace wapanel::applet {

volume_control::volume_control(wap_t_applet_config applet_config, backend *backend)
	: m_backend(backend)
	, m_toggle_btn(GTK_TOGGLE_BUTTON(gtk_toggle_button_new())) {

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

	log_info("Resolved config");

	// Rest of config stuff

	if (m_config.is_flat) gtk_button_set_relief(GTK_BUTTON(m_toggle_btn), GTK_RELIEF_NONE);

	unsigned int icon_size = m_config.__panel_height * 1.5;
	if (m_config.icon_height != -1) {
		icon_size = m_config.icon_height;
	} else {
		m_config.icon_height = icon_size;
	}

	// GTK

	m_button_icon = GTK_IMAGE(
		gtk_image_new_from_pixbuf(ic::get_icon("audio-volume-muted-symbolic", m_config.icon_height)));

	gtk_container_add(GTK_CONTAINER(this->m_toggle_btn), GTK_WIDGET(this->m_button_icon));

	// Some backend tests

	m_backend->callback_output_volume_changed([=](float volume) {
		if (volume == 0) {
			gtk_image_set_from_pixbuf(
				m_button_icon,
				ic::get_icon("audio-volume-muted-symbolic", m_config.icon_height));
		} else if (volume <= 33) {
			gtk_image_set_from_pixbuf(
				m_button_icon,
				ic::get_icon("audio-volume-low-symbolic", m_config.icon_height));
		} else if (volume <= 66 && volume > 33) {
			gtk_image_set_from_pixbuf(
				m_button_icon,
				ic::get_icon("audio-volume-medium-symbolic", m_config.icon_height));
		} else if (volume > 66) {
			gtk_image_set_from_pixbuf(
				m_button_icon,
				ic::get_icon("audio-volume-high-symbolic", m_config.icon_height));
		}
	});

	m_backend->callback_input_mute_changed([](bool is_muted) { printf("Muted input!!!\n"); });

	m_backend->callback_input_volume_changed([](float volume) { printf("Changed volume in input!!!\n"); });

	m_backend->callback_output_mute_changed([](bool is_muted) { printf("Muted output!!!\n"); });
}
volume_control::~volume_control() {}

auto volume_control::get_widget() -> GtkWidget * { return GTK_WIDGET(this->m_toggle_btn); }

}