#pragma once
#include <array>
#include <gtk/gtk.h>
#include <string>

namespace ui_comps {

class logout_box {
private:
	GtkBox *m_root;
	std::array<GtkButton *, 6> m_action_buttons;
	std::array<GtkBox *, 6> m_action_btn_container;

public:
	struct config {
		std::string shutdown_cmd;
		std::string restart_cmd;
		std::string logout_cmd;
		std::string suspend_cmd;
		std::string hibernate_cmd;
		std::string lock_cmd;

		bool is_anything_there = false;
	};

	logout_box(config conf);
	~logout_box();

	auto get_widget() -> GtkWidget *;

private:
	config cmp_config;
};

}