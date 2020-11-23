#pragma once

#include "wl_toplevel.hh"
#include "wl_toplevel_manager.hh"
#include <appletapi.h>
#include <gtk/gtk.h>
#include <unordered_map>

/*
	Standard configuration

	[[panel.applet]]
	name = "task-switcher"
	[panel.applet.mode.scroll]
		fallback_icon = "application-x-executable"
		expand = true
		minimum_windows_per_row = 6
		maximum_windows_per_row = 12
		[panel.applet.mode.scroll.button]
			icon_only = false
			gap = 8
			flat = false
			icon_height = -1
*/

namespace wapanel::applet {

class task_switcher;

class window_button {
private:
	GtkFlowBoxChild *m_flow_box_child;
	GtkToggleButton *m_button;
	GtkBox *m_aligment_box;
	GtkImage *m_icon;
	GtkLabel *m_title;

	// GtkMenu* m_menu;

	wl::toplevel *m_toplevel;
	task_switcher *m_task_switcher;
	bool m_button_initialized = false;

	struct button_toggled_data {
		wl::toplevel *toplevel;
		wl::toplevel_manager *toplevel_manager;
	} * m_button_toggled_data;

	struct button_click_event_data {
		wl::toplevel *toplevel;
		class task_switcher *task_switcher;
	} * m_button_click_event_data;

	auto search_for_icon(std::string app_id) -> std::string;

public:
	window_button(wl::toplevel *wl_toplevel, task_switcher *task_switcher);
	~window_button();

	auto get_widget() -> GtkWidget *;
	auto toplevel_event_handler(wl::toplevel_event event) -> void;
};

class task_switcher {
private:
	GtkFlowBox *m_window_button_container;
	GtkScrolledWindow *m_scroll_window;
	int m_id;

	GtkMenu *m_context_menu = NULL;

	std::unordered_map<unsigned int, window_button *> m_buttons;

public:
	task_switcher(wap_t_applet_config applet_config, int id);
	~task_switcher();

	auto get_widget() -> GtkWidget *;

	auto window_button_ready(wl::toplevel *wl_toplevel) -> void;
	auto window_button_click_event(GdkEvent *event, wl::toplevel *toplevel) -> void;

	struct config {
		unsigned int __panel_height = 16;

		struct scroll {
			std::string fallback_icon = "application-x-executable";
			bool expand = true;
			unsigned int minimum_windows_per_row = 6;
			unsigned int maximum_windows_per_row = 12;

			struct button {
				bool icon_only = false;
				unsigned int gap = 8;
				bool flat = false;
				int icon_height = -1;
			} button;
		} scroll;
	} config;
};

}