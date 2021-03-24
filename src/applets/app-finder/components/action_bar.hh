#pragma once
#include "./logout_box.hh"
#include <gtk/gtk.h>
#include <string>

namespace ui_comps {

class action_bar {
private:
	GtkBox *m_root;
	GtkButton *m_user_account_setting;

	GtkBox *m_fast_actions_container;
	GtkButton *m_system_settings;
	GtkButton *m_file_manager;
	GtkMenuButton *m_logout;
	GtkPopover *m_logout_popover;

	logout_box *m_logout_content;

public:
	struct config {
		std::string user_manager_cmd;
		std::string settings_cmd;
		std::string file_manager_cmd;
	};

	action_bar(action_bar::config cmp_conf, logout_box::config logout_conf, int apid);
	~action_bar();

	auto get_widget() -> GtkWidget *;

private:
	config cmp_config;
};

};
