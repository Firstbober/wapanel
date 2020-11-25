#include "app-finder.hh"
#include "../icon_cache.hh"
#include <functional>

namespace wapanel::applet {

namespace ui_comps {

	logout_box::logout_box()
		: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 4))) {}
	logout_box::~logout_box() {}
	auto logout_box::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }

	action_bar::action_bar()
		: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0))) {}
	action_bar::~action_bar() {}
	auto action_bar::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }

	list_area::list_area() {}
	list_area::~list_area() {}
	auto list_area::get_widget() -> GtkWidget * { return GTK_WIDGET(m_list_container); }

};

app_finder::app_finder(wap_t_applet_config applet_config, int id)
	: m_id(id)
	, m_app_finder_btn(GTK_MENU_BUTTON(gtk_menu_button_new()))
	, m_btn_icon(GTK_IMAGE(gtk_image_new()))
	, m_finder_popover(GTK_POPOVER(gtk_popover_new(GTK_WIDGET(m_app_finder_btn))))
	, m_finder_aligner(GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4))) {

	// First part of config

	_wap_t_config_variable *var;

	if (wapi_key_exists(&applet_config.root, "__panel_height")) {
		var = wapi_get_var_from_table(&applet_config.root, "__panel_height");
		m_config.__panel_height = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "icon_height")) {
		var = wapi_get_var_from_table(&applet_config.root, "icon_height");
		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) m_config.icon_height = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "flat")) {
		var = wapi_get_var_from_table(&applet_config.root, "flat");
		if (var->type == WAP_CONF_VAR_TYPE_BOOLEAN) m_config.is_flat = wapi_var_as_boolean(var);
	}

	// Second part of config

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

	// Third part of config

	ui_comps::logout_box::config logout_box_cfg;

	if (wapi_key_exists(&applet_config.root, "logout")) {
		_wap_t_config_variable *config_logout = wapi_get_var_from_table(&applet_config.root, "logout");

		if (wapi_key_exists(config_logout, "shutdown")) {
			var = wapi_get_var_from_table(config_logout, "shutdown");
			if (var->type == WAP_CONF_VAR_TYPE_STRING)
				logout_box_cfg.shutdown_cmd = std::string(wapi_var_as_string(var));
		}

		std::array<std::string, 6> keys = { "shutdown", "restart", "logout", "suspend", "hibernate", "lock" };
		std::array<std::reference_wrapper<std::string>, 6> var_ref
			= { logout_box_cfg.shutdown_cmd, logout_box_cfg.restart_cmd,   logout_box_cfg.logout_cmd,
				logout_box_cfg.suspend_cmd,	 logout_box_cfg.hibernate_cmd, logout_box_cfg.lock_cmd };

		for (size_t i = 0; i < keys.size(); i++) {
			var = wapi_get_var_from_table(config_logout, keys[i].c_str());
			if (var->type == WAP_CONF_VAR_TYPE_STRING) var_ref[i].get() = std::string(wapi_var_as_string(var));
		}
	}

	if (m_config.icon_height == -1) { m_config.icon_height = m_config.__panel_height * 1.5; }

	log_info("Resolved config");

	// Gtk styling and other stuff

	//  Panel button and icon
	gtk_image_set_from_pixbuf(m_btn_icon, utils::ic::get_icon("wapa-logo-nocolor-dark", m_config.icon_height));
	if (m_config.is_flat) gtk_button_set_relief(GTK_BUTTON(m_app_finder_btn), GTK_RELIEF_NONE);

	gtk_container_add(GTK_CONTAINER(m_app_finder_btn), GTK_WIDGET(m_btn_icon));

	GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_app_finder_btn));
	gtk_style_context_add_class(context, "app-finder");
	gtk_widget_set_name(GTK_WIDGET(m_app_finder_btn), std::string("app-finder-" + std::to_string(m_id)).c_str());
}
app_finder::~app_finder() {}

auto app_finder::get_widget() -> GtkWidget * { return GTK_WIDGET(m_app_finder_btn); }

} // namespace wapanel::applet
