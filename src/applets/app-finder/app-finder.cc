#include "app-finder.hh"
#include "../../log.hh"
#include "../double_fork.hh"
#include "../icon_cache.hh"
#include "menu_entries.hh"
#include <functional>

namespace wapanel::applet {

namespace ui_comps {

	logout_box::logout_box(config conf)
		: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 4)))
		, cmp_config(conf) {

		std::array<std::reference_wrapper<std::string>, 6> vars_to_check
			= { cmp_config.shutdown_cmd, cmp_config.restart_cmd,   cmp_config.logout_cmd,
				cmp_config.suspend_cmd,	 cmp_config.hibernate_cmd, cmp_config.lock_cmd };
		std::array<std::string, 6> labels = { "Shutdown", "Restart", "Logout", "Suspend", "Hibernate", "Lock" };
		std::array<std::string, 6> icons
			= { "system-shutdown-symbolic", "system-reboot-symbolic", "system-log-out-symbolic",
				"night-light-symbolic",		"process-stop-symbolic",  "system-lock-screen-symbolic" };

		for (size_t i = 0; i < vars_to_check.size(); i++) {
			if (vars_to_check[i].get() != "") {
				m_action_buttons[i] = GTK_BUTTON(gtk_button_new());
				m_action_btn_container[i] = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));

				gtk_button_set_relief(m_action_buttons[i], GTK_RELIEF_NONE);

				gtk_box_pack_start(m_action_btn_container[i],
								   GTK_WIDGET(gtk_image_new_from_pixbuf(utils::ic::get_icon(icons[i], 20))), false,
								   true, 0);

				gtk_box_pack_start(m_action_btn_container[i], GTK_WIDGET(gtk_label_new(labels[i].c_str())), false, true,
								   0);

				gtk_container_add(GTK_CONTAINER(m_action_buttons[i]), GTK_WIDGET(m_action_btn_container[i]));
				gtk_box_pack_start(m_root, GTK_WIDGET(m_action_buttons[i]), false, true, 0);

				g_signal_connect(m_action_buttons[i], "clicked", G_CALLBACK(+[](GtkButton *btn, std::string *cmd) {
									 g_spawn_command_line_async(std::string("sh -c \"" + *cmd + "\"").c_str(), NULL);
								 }),
								 &vars_to_check[i].get());
			}
		}
	}
	logout_box::~logout_box() {}
	auto logout_box::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }

	action_bar::action_bar(action_bar::config conf, logout_box::config logout_conf, int apid)
		: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)))
		, m_fast_actions_container(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8)))
		, m_logout_content(new logout_box(logout_conf))
		, cmp_config(conf) {

		if (cmp_config.user_manager_cmd != "") {
			m_user_account_setting = GTK_BUTTON(gtk_button_new());
			gtk_container_add(GTK_CONTAINER(m_user_account_setting),
							  GTK_WIDGET(gtk_image_new_from_pixbuf(utils::ic::get_icon("wapa-account", 24))));

			g_signal_connect(m_user_account_setting, "clicked", G_CALLBACK(+[](GtkButton *btn, std::string *cmd) {
								 g_spawn_command_line_async(std::string("sh -c \"" + *cmd + "\"").c_str(), NULL);
							 }),
							 &cmp_config.user_manager_cmd);

			gtk_button_set_relief(m_user_account_setting, GTK_RELIEF_NONE);
			gtk_box_pack_start(m_root, GTK_WIDGET(m_user_account_setting), false, true, 0);
		}

		if (cmp_config.settings_cmd != "") {
			m_system_settings = GTK_BUTTON(gtk_button_new());
			gtk_container_add(GTK_CONTAINER(m_system_settings),
							  GTK_WIDGET(gtk_image_new_from_pixbuf(utils::ic::get_icon("wapa-cog", 24))));

			g_signal_connect(m_system_settings, "clicked", G_CALLBACK(+[](GtkButton *btn, std::string *cmd) {
								 g_spawn_command_line_async(std::string("sh -c \"" + *cmd + "\"").c_str(), NULL);
							 }),
							 &cmp_config.settings_cmd);

			gtk_button_set_relief(m_system_settings, GTK_RELIEF_NONE);
			gtk_box_pack_start(m_fast_actions_container, GTK_WIDGET(m_system_settings), false, true, 0);
		}

		if (cmp_config.file_manager_cmd != "") {
			m_file_manager = GTK_BUTTON(gtk_button_new());
			gtk_container_add(GTK_CONTAINER(m_file_manager),
							  GTK_WIDGET(gtk_image_new_from_pixbuf(utils::ic::get_icon("wapa-folder", 24))));

			g_signal_connect(m_file_manager, "clicked", G_CALLBACK(+[](GtkButton *btn, std::string *cmd) {
								 g_spawn_command_line_async(std::string("sh -c \"" + *cmd + "\"").c_str(), NULL);
							 }),
							 &cmp_config.file_manager_cmd);

			gtk_button_set_relief(m_file_manager, GTK_RELIEF_NONE);
			gtk_box_pack_start(m_fast_actions_container, GTK_WIDGET(m_file_manager), false, true, 0);
		}

		if (logout_conf.is_anything_there) {
			m_logout = GTK_MENU_BUTTON(gtk_menu_button_new());
			m_logout_popover = GTK_POPOVER(gtk_popover_new(GTK_WIDGET(m_logout)));

			gtk_container_add(
				GTK_CONTAINER(m_logout),
				GTK_WIDGET(gtk_image_new_from_pixbuf(utils::ic::get_icon("system-log-out-symbolic", 24))));

			gtk_button_set_relief(GTK_BUTTON(m_logout), GTK_RELIEF_NONE);

			gtk_container_add(GTK_CONTAINER(m_logout_popover), GTK_WIDGET(m_logout_content->get_widget()));
			gtk_widget_show_all(m_logout_content->get_widget());

			gtk_menu_button_set_popover(m_logout, GTK_WIDGET(m_logout_popover));

			GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_logout_popover));
			gtk_style_context_add_class(context, "app-finder-logout-popover");
			gtk_widget_set_name(GTK_WIDGET(m_logout_popover),
								std::string("app-finder-logout-popover-" + std::to_string(apid)).c_str());

			gtk_box_pack_start(m_fast_actions_container, GTK_WIDGET(m_logout), false, true, 0);
		}

		gtk_box_pack_end(m_root, GTK_WIDGET(m_fast_actions_container), false, true, 0);
	}
	action_bar::~action_bar() {
		gtk_popover_popdown(m_logout_popover);
		delete m_logout_content;
	}
	auto action_bar::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }

	list_area::list_area()
		: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 4)))
		, m_list_container(GTK_NOTEBOOK(gtk_notebook_new())) {

		menu_entries::get();
		// TODO: Make this work
	}
	list_area::~list_area() {}
	auto list_area::get_widget() -> GtkWidget * { return GTK_WIDGET(m_list_container); }
};

app_finder::app_finder(wap_t_applet_config applet_config, int id)
	: m_id(id)
	, m_app_finder_btn(GTK_MENU_BUTTON(gtk_menu_button_new()))
	, m_btn_icon(GTK_IMAGE(gtk_image_new()))
	, m_finder_popover(GTK_POPOVER(gtk_popover_new(GTK_WIDGET(m_app_finder_btn))))
	, m_finder_aligner(GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4))) {

	// 1st part of config

	_wap_t_config_variable *var;

	if (wapi_key_exists(&applet_config.root, "__panel_height")) {
		var = wapi_get_var_from_table(&applet_config.root, "__panel_height");
		m_config.__panel_height = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "icon")) {
		var = wapi_get_var_from_table(&applet_config.root, "icon");
		if (var->type == WAP_CONF_VAR_TYPE_STRING) m_config.icon_name = std::string(wapi_var_as_string(var));
	}

	if (wapi_key_exists(&applet_config.root, "icon_height")) {
		var = wapi_get_var_from_table(&applet_config.root, "icon_height");
		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) m_config.icon_height = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "flat")) {
		var = wapi_get_var_from_table(&applet_config.root, "flat");
		if (var->type == WAP_CONF_VAR_TYPE_BOOLEAN) m_config.is_flat = wapi_var_as_boolean(var);
	}

	// 2nd part of config

	if (wapi_key_exists(&applet_config.root, "popover_width")) {
		var = wapi_get_var_from_table(&applet_config.root, "popover_width");
		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) m_config.popover_width = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "popover_height")) {
		var = wapi_get_var_from_table(&applet_config.root, "popover_height");
		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) m_config.popover_height = wapi_var_as_integer(var);
	}

	// 3rd part of config

	ui_comps::action_bar::config action_bar_cfg;

	if (wapi_key_exists(&applet_config.root, "user_manager")) {
		var = wapi_get_var_from_table(&applet_config.root, "user_manager");
		if (var->type == WAP_CONF_VAR_TYPE_STRING)
			action_bar_cfg.user_manager_cmd = std::string(wapi_var_as_string(var));
	}

	if (wapi_key_exists(&applet_config.root, "settings")) {
		var = wapi_get_var_from_table(&applet_config.root, "settings");
		if (var->type == WAP_CONF_VAR_TYPE_STRING) action_bar_cfg.settings_cmd = std::string(wapi_var_as_string(var));
	}

	if (wapi_key_exists(&applet_config.root, "file_manager")) {
		var = wapi_get_var_from_table(&applet_config.root, "file_manager");
		if (var->type == WAP_CONF_VAR_TYPE_STRING)
			action_bar_cfg.file_manager_cmd = std::string(wapi_var_as_string(var));
	}

	// 4th part of config

	ui_comps::logout_box::config logout_box_cfg;

	if (wapi_key_exists(&applet_config.root, "logout")) {
		_wap_t_config_variable *config_logout = wapi_get_var_from_table(&applet_config.root, "logout");

		std::array<std::string, 6> keys = { "shutdown", "restart", "logout", "suspend", "hibernate", "lock" };
		std::array<std::reference_wrapper<std::string>, 6> var_ref
			= { logout_box_cfg.shutdown_cmd, logout_box_cfg.restart_cmd,   logout_box_cfg.logout_cmd,
				logout_box_cfg.suspend_cmd,	 logout_box_cfg.hibernate_cmd, logout_box_cfg.lock_cmd };

		for (size_t i = 0; i < keys.size(); i++) {
			var = wapi_get_var_from_table(config_logout, keys[i].c_str());
			if (var->type == WAP_CONF_VAR_TYPE_STRING) {
				var_ref[i].get() = std::string(wapi_var_as_string(var));
				if (var_ref[i].get() != "") logout_box_cfg.is_anything_there = true;
			}
		}
	}

	if (m_config.icon_height == -1) { m_config.icon_height = m_config.__panel_height * 1.5; }

	log_info("Resolved config");

	// Subclass initialization

	m_sidebar = new ui_comps::action_bar(action_bar_cfg, logout_box_cfg, m_id);
	m_list_area = new ui_comps::list_area();

	// Gtk styling and other stuff

	//  Panel button and icon
	GdkPixbuf *button_icon_pix = NULL;
	if (m_config.icon_name != "") {
		button_icon_pix = utils::ic::get_icon(m_config.icon_name, m_config.icon_height);
		if (button_icon_pix == NULL)
			button_icon_pix
				= gdk_pixbuf_new_from_file_at_size(m_config.icon_name.c_str(), -1, m_config.icon_height, NULL);
	}

	if (button_icon_pix == NULL) button_icon_pix = utils::ic::get_icon("wapa-logo-nocolor-dark", m_config.icon_height);

	gtk_image_set_from_pixbuf(m_btn_icon, button_icon_pix);
	if (m_config.is_flat) gtk_button_set_relief(GTK_BUTTON(m_app_finder_btn), GTK_RELIEF_NONE);

	gtk_container_add(GTK_CONTAINER(m_app_finder_btn), GTK_WIDGET(m_btn_icon));
	gtk_menu_button_set_popover(m_app_finder_btn, GTK_WIDGET(m_finder_popover));

	GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_app_finder_btn));
	gtk_style_context_add_class(context, "app-finder");
	gtk_widget_set_name(GTK_WIDGET(m_app_finder_btn), std::string("app-finder-" + std::to_string(m_id)).c_str());

	//  Popover
	gtk_container_add(GTK_CONTAINER(m_finder_popover), GTK_WIDGET(m_finder_aligner));

	context = gtk_widget_get_style_context(GTK_WIDGET(m_finder_popover));
	gtk_style_context_add_class(context, "app-finder-popover");
	gtk_widget_set_name(GTK_WIDGET(m_app_finder_btn),
						std::string("app-finder-popover-" + std::to_string(m_id)).c_str());

	int popover_width_to_use = 535;
	int popover_height_to_use = 570;

	if (m_config.popover_width != -1) popover_width_to_use = m_config.popover_width;
	if (m_config.popover_height != -1) popover_height_to_use = m_config.popover_height;

	gtk_widget_set_size_request(GTK_WIDGET(m_finder_popover), popover_width_to_use, popover_height_to_use);

	gtk_box_pack_start(m_finder_aligner, m_sidebar->get_widget(), false, true, 0);
	gtk_box_pack_start(m_finder_aligner, gtk_separator_new(GTK_ORIENTATION_VERTICAL), false, true, 0);
	gtk_box_pack_start(m_finder_aligner, m_list_area->get_widget(), true, true, 0);

	gtk_widget_set_margin_top(GTK_WIDGET(m_finder_aligner), 4);
	gtk_widget_set_margin_bottom(GTK_WIDGET(m_finder_aligner), 4);
	gtk_widget_set_margin_start(GTK_WIDGET(m_finder_aligner), 4);
	gtk_widget_set_margin_end(GTK_WIDGET(m_finder_aligner), 4);

	gtk_widget_show_all(GTK_WIDGET(m_finder_aligner));
}
app_finder::~app_finder() {
	gtk_popover_popdown(m_finder_popover);

	delete m_sidebar;
	delete m_list_area;

	log_info("Destroyed app-finder");
}

auto app_finder::get_widget() -> GtkWidget * { return GTK_WIDGET(m_app_finder_btn); }

} // namespace wapanel::applet
