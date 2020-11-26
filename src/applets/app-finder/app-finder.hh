#pragma once
#include <appletapi.h>
#include <gtk/gtk.h>
#include <string>

/*
	Default config

	[[panel.applet]]
		name = "app-finder"

		icon = ""
		icon_height = -1
		flat = false

		popover_width = -1
		popover_height = -1

		user_manager = ""
		settings = ""
		file_manager = ""

		[panel.applet.logout]
			shutdown = ""
			restart = ""
			logout = ""
			suspend = ""
			hibernate = ""
			lock = ""
*/

namespace wapanel::applet {

namespace ui_comps {

	class logout_box {
	private:
		GtkBox *m_root;

	public:
		struct config {
			std::string shutdown_cmd;
			std::string restart_cmd;
			std::string logout_cmd;
			std::string suspend_cmd;
			std::string hibernate_cmd;
			std::string lock_cmd;
		};

		logout_box(config conf);
		~logout_box();

		auto get_widget() -> GtkWidget *;

	private:
		config cmp_config;
	};

	class action_bar {
	private:
		GtkBox *m_root;
		GtkButton *m_user_account_setting;

		GtkBox *m_fast_actions_container;
		GtkButton *m_system_settings;
		GtkButton *m_file_manager;
		GtkMenuButton *m_logout;
		GtkPopover *m_logout_popover;

		logout_box m_logout_content;

	public:
		struct config {
			std::string user_manager_cmd;
			std::string settings_cmd;
			std::string file_manager_cmd;
		};

		action_bar(action_bar::config cmp_conf, logout_box::config logout_conf);
		~action_bar();

		auto get_widget() -> GtkWidget *;

	private:
		config cmp_config;
	};

	class list_area {
	private:
		GtkBox *m_root;
		GtkNotebook *m_list_container;

	public:
		list_area();
		~list_area();

		auto get_widget() -> GtkWidget *;
	};

};

class app_finder {
private:
	int m_id;

	GtkMenuButton *m_app_finder_btn;
	GtkImage *m_btn_icon;
	GtkPopover *m_finder_popover;
	GtkBox *m_finder_aligner;

	ui_comps::action_bar *m_sidebar;
	ui_comps::list_area *m_list_area;

	struct config {
		int __panel_height;

		std::string icon_name;
		int icon_height = -1;
		bool is_flat;

		int popover_width = -1;
		int popover_height = -1;
	} m_config;

public:
	app_finder(wap_t_applet_config applet_config, int id);
	~app_finder();

	auto get_widget() -> GtkWidget *;
};

} // namespace wapanel::applet
