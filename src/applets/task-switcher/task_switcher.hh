#pragma once

#include "wl_toplevel.hh"
#include "wl_toplevel_manager.hh"
#include <appletapi.h>
#include <gtk/gtk.h>
#include <unordered_map>

namespace wapanel::applet {

class task_switcher;

class window_button {
private:
	GtkFlowBoxChild *m_flow_box_child;
	GtkToggleButton *m_button;
	GtkBox *m_aligment_box;
	GtkImage *m_icon;
	GtkLabel *m_title;

	wl::toplevel *m_toplevel;
	task_switcher *m_task_switcher;
	bool m_button_initialized = false;

	struct button_toggled_data {
		wl::toplevel *toplevel;
		wl::toplevel_manager* toplevel_manager;
	} * m_button_toggled_data;

	auto toplevel_event_handler(wl::toplevel_event event) -> void;
	auto search_for_icon(std::string app_id) -> std::string;

public:
	window_button(wl::toplevel *wl_toplevel, task_switcher *task_switcher);
	~window_button();

	auto get_widget() -> GtkWidget *;
};

class task_switcher {
private:
	GtkFlowBox *m_window_button_container;
	GtkScrolledWindow *m_scroll_window;

	std::unordered_map<unsigned int, window_button *> m_buttons;

public:
	task_switcher(wap_t_applet_config applet_config);
	~task_switcher();

	auto get_widget() -> GtkWidget *;

	auto window_button_ready(wl::toplevel *wl_toplevel) -> void;
};

}