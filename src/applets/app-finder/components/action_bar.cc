#include "action_bar.hh"
#include "../../icon_cache.hh"

namespace ui_comps {
	action_bar::action_bar(action_bar::config conf, logout_box::config logout_conf, int apid)
		: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)))
		, m_fast_actions_container(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8)))
		, m_logout_content(new logout_box(logout_conf))
		, cmp_config(conf) {

		if (cmp_config.user_manager_cmd != "") {
			m_user_account_setting = GTK_BUTTON(gtk_button_new());
			gtk_container_add(GTK_CONTAINER(m_user_account_setting),
							  GTK_WIDGET(gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon("wapa-account", 24))));

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
							  GTK_WIDGET(gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon("wapa-cog", 24))));

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
							  GTK_WIDGET(gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon("wapa-folder", 24))));

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
				GTK_WIDGET(gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon("system-log-out-symbolic", 24))));

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
};