#include "task_switcher.hh"
#include "../../log.hh"
#include "gio/gmenu.h"
#include "gmodule.h"
#include "wl_toplevel.hh"
#include "wl_toplevel_manager.hh"
#include "wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"
#include <filesystem>
#include <gio/gdesktopappinfo.h>
#include <stdexcept>

#define DEFAULT_APP_ICON "application-x-executable"

namespace wapanel::applet {

task_switcher::task_switcher(wap_t_applet_config applet_config) {
	wl::toplevel_manager::get().try_to_initialize();

	wl::toplevel_manager::get().on_toplevel_new(
		[&](wl::toplevel *wl_toplevel) { m_buttons[wl_toplevel->mgid] = new window_button(wl_toplevel, this); });

	wl::toplevel_manager::get().on_toplevel_finished([&](wl::toplevel *wl_toplevel) {
		gtk_container_remove(GTK_CONTAINER(m_window_button_container), m_buttons[wl_toplevel->mgid]->get_widget());

		delete m_buttons[wl_toplevel->mgid];
		m_buttons.erase(wl_toplevel->mgid);

		log_info("Removed window_button");
	});

	log_info("Created handles for wl_toplevel_manager events");

	// Basic variable initialization

	m_scroll_window = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));

	gtk_scrolled_window_set_policy(m_scroll_window, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(m_scroll_window, GTK_SHADOW_NONE);
	gtk_scrolled_window_set_propagate_natural_width(m_scroll_window, false);
	gtk_scrolled_window_set_propagate_natural_height(m_scroll_window, false);

	m_window_button_container = GTK_FLOW_BOX(gtk_flow_box_new());
	gtk_flow_box_set_homogeneous(m_window_button_container, true);
	gtk_flow_box_set_min_children_per_line(m_window_button_container, 12);
	gtk_flow_box_set_selection_mode(m_window_button_container, GTK_SELECTION_NONE);

	gtk_container_add(GTK_CONTAINER(m_scroll_window), GTK_WIDGET(m_window_button_container));
	gtk_scrolled_window_set_overlay_scrolling(m_scroll_window, false);

	log_info("Spawned variables for task_switcher");

	// Config resolving
	/*
	_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "__panel_height");
	int64_t __panel_height = wapi_var_as_integer(var);
	*/
	log_info("Initialized config variables");
}
task_switcher::~task_switcher() {}

auto task_switcher::get_widget() -> GtkWidget * { return GTK_WIDGET(this->m_scroll_window); }

auto task_switcher::window_button_ready(wl::toplevel *wl_toplevel) -> void {
	gtk_container_add(GTK_CONTAINER(m_window_button_container), m_buttons[wl_toplevel->mgid]->get_widget());
	gtk_widget_show_all(GTK_WIDGET(m_window_button_container));
}

auto task_switcher::window_button_click_event(GdkEvent *event, wl::toplevel *toplevel) -> void {
	GdkEventButton *event_button;
	if (event->type == GDK_BUTTON_PRESS) {
		event_button = (GdkEventButton *)event;
		if (event_button->button == GDK_BUTTON_SECONDARY) {
			m_context_menu = GTK_MENU(gtk_menu_new());
			GtkWidget *item;

			auto on_minimize = +[](GtkMenuItem *menu_item, wl::toplevel *toplevel) { toplevel->set_minimized(); };
			auto on_maximize = +[](GtkMenuItem *menu_item, wl::toplevel *toplevel) { toplevel->set_maximized(); };
			auto on_unminimize = +[](GtkMenuItem *menu_item, wl::toplevel *toplevel) { toplevel->unset_minimized(); };
			auto on_unmaximize = +[](GtkMenuItem *menu_item, wl::toplevel *toplevel) { toplevel->unset_maximized(); };
			auto on_close = +[](GtkMenuItem *menu_item, wl::toplevel *toplevel) { toplevel->close(); };
			auto on_show = +[](GtkMenuItem *menu_item, wl::toplevel *toplevel) { toplevel->set_activated(); };

			if (toplevel->mgid == wl::toplevel_manager::get().current_window) {
				item = gtk_menu_item_new_with_label("Minimize");
				g_signal_connect(item, "activate", G_CALLBACK(on_minimize), toplevel);
				gtk_container_add(GTK_CONTAINER(m_context_menu), item);

				item = gtk_menu_item_new_with_label("Maximize");
				g_signal_connect(item, "activate", G_CALLBACK(on_maximize), toplevel);
				gtk_container_add(GTK_CONTAINER(m_context_menu), item);
			} else if (toplevel->state == wl::toplevel_state::MINIMIZED) {
				item = gtk_menu_item_new_with_label("Unminimize");
				g_signal_connect(item, "activate", G_CALLBACK(on_unminimize), toplevel);
				gtk_container_add(GTK_CONTAINER(m_context_menu), item);
			}

			item = gtk_separator_menu_item_new();
			gtk_container_add(GTK_CONTAINER(m_context_menu), item);

			item = gtk_menu_item_new_with_label("Close");
			g_signal_connect(item, "activate", G_CALLBACK(on_close), toplevel);
			gtk_container_add(GTK_CONTAINER(m_context_menu), item);

			gtk_widget_show_all(GTK_WIDGET(m_context_menu));
			gtk_menu_popup_at_pointer(GTK_MENU(m_context_menu), event);
		}
	}
}

// Window button

window_button::window_button(wl::toplevel *wl_toplevel, task_switcher *task_switcher) {
	m_toplevel = wl_toplevel;
	m_task_switcher = task_switcher;

	m_toplevel->on_event([&](wl::toplevel_event event) { this->toplevel_event_handler(event); });

	m_flow_box_child = GTK_FLOW_BOX_CHILD(gtk_flow_box_child_new());
	gtk_widget_set_hexpand(GTK_WIDGET(m_flow_box_child), true);
	gtk_widget_set_halign(GTK_WIDGET(m_flow_box_child), GTK_ALIGN_FILL);
	gtk_widget_set_valign(GTK_WIDGET(m_flow_box_child), GTK_ALIGN_CENTER);

	m_button = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
	m_aligment_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));

	gtk_widget_set_focus_on_click(GTK_WIDGET(m_button), false);

	m_button_toggled_data = new button_toggled_data { this->m_toplevel, &wl::toplevel_manager::get() };

	g_signal_connect(
		m_button, "toggled", G_CALLBACK(+[](GtkToggleButton *toggle_button, button_toggled_data *btn_toggled_data) {
			if (gtk_toggle_button_get_active(toggle_button)) {
				btn_toggled_data->toplevel->unset_minimized();
			} else {
				if (btn_toggled_data->toplevel->mgid == btn_toggled_data->toplevel_manager->current_window) {
					btn_toggled_data->toplevel->set_minimized();
				} else {
					btn_toggled_data->toplevel->set_activated();
				}
			}
		}),
		m_button_toggled_data);

	m_button_click_event_data = new button_click_event_data { this->m_toplevel, this->m_task_switcher };

	g_signal_connect(m_button, "button_press_event",
					 G_CALLBACK(+[](GtkWidget *widget, GdkEvent *event, button_click_event_data *data) {
						 data->task_switcher->window_button_click_event(event, data->toplevel);
						 return false;
					 }),
					 m_button_click_event_data);

	log_info("Connected m_button events");

	gtk_container_add(GTK_CONTAINER(m_button), GTK_WIDGET(m_aligment_box));
	gtk_container_add(GTK_CONTAINER(m_flow_box_child), GTK_WIDGET(m_button));

	log_info("Created window_button");
}
window_button::~window_button() {
	delete this->m_button_toggled_data;
	delete this->m_button_click_event_data;
}

auto window_button::get_widget() -> GtkWidget * { return GTK_WIDGET(m_flow_box_child); }

auto window_button::toplevel_event_handler(wl::toplevel_event event) -> void {
	switch (event) {
	case wl::toplevel_event::DONE:
		if (!m_button_initialized) {
			GtkIconTheme *default_icon_theme = gtk_icon_theme_get_default();
			GdkPixbuf *rendered_icon = NULL;
			int icon_height = 24;

			if (gtk_icon_theme_has_icon(default_icon_theme, m_toplevel->app_id.c_str())) {
				rendered_icon = gtk_icon_theme_load_icon(default_icon_theme, m_toplevel->app_id.c_str(), icon_height,
														 GTK_ICON_LOOKUP_FORCE_REGULAR, NULL);
			} else {
				log_warn("app_id is not matching any icon in icon theme, searching in system paths");
				std::string icon_name = search_for_icon(m_toplevel->app_id.c_str());

				if (!gtk_icon_theme_has_icon(default_icon_theme, icon_name.c_str())) {
					log_warn("app_id doesn't have any defined icon available in icon theme, setting default icon");
					icon_name = DEFAULT_APP_ICON;
				}

				rendered_icon = gtk_icon_theme_load_icon(default_icon_theme, icon_name.c_str(), icon_height,
														 GTK_ICON_LOOKUP_FORCE_REGULAR, NULL);
			}

			m_icon = GTK_IMAGE(gtk_image_new_from_pixbuf(rendered_icon));
			m_title = GTK_LABEL(gtk_label_new(m_toplevel->title.c_str()));

			gtk_label_set_line_wrap(m_title, true);
			gtk_label_set_line_wrap_mode(m_title, PANGO_WRAP_CHAR);
			gtk_label_set_ellipsize(m_title, PANGO_ELLIPSIZE_END);

			gtk_box_pack_start(m_aligment_box, GTK_WIDGET(m_icon), false, true, 0);
			gtk_box_pack_start(m_aligment_box, GTK_WIDGET(m_title), false, true, 0);

			this->m_task_switcher->window_button_ready(this->m_toplevel);
			m_button_initialized = true;
		}
		{
			GValue val = G_VALUE_INIT;

			if (m_toplevel->state == wl::toplevel_state::ACTIVATED
				|| m_toplevel->state == wl::toplevel_state::MAXIMIZED) {
				g_object_get_property(G_OBJECT(m_button), "active", &val);
				g_value_set_boolean(&val, true);
				g_object_set_property(G_OBJECT(m_button), "active", &val);
				gtk_label_set_label(m_title, m_toplevel->title.c_str());
			} else {
				g_object_get_property(G_OBJECT(m_button), "active", &val);
				g_value_set_boolean(&val, false);
				g_object_set_property(G_OBJECT(m_button), "active", &val);
				gtk_label_set_label(m_title, ("[" + m_toplevel->title + "]").c_str());
			}
		}
		break;
	case wl::toplevel_event::TITLE_CHANGE:
		gtk_widget_set_tooltip_text(GTK_WIDGET(m_button), m_toplevel->title.c_str());
		break;

	default:
		break;
	}
}

auto window_button::search_for_icon(std::string app_id) -> std::string {
	std::string icon_name = "";

	std::string app_id_lowercase = app_id;
	std::transform(app_id_lowercase.begin(), app_id_lowercase.end(), app_id_lowercase.begin(), ::toupper);

	for (auto &&directory_path : { "/usr/share/applications/", "/usr/local/share/applications/" }) {
		if (icon_name.length() > 0) { break; }

		for (auto &&directory_entry : std::filesystem::directory_iterator(directory_path)) {
			if (!directory_entry.is_regular_file()) continue;

			if (directory_entry.path().stem() == app_id || directory_entry.path().stem() == app_id_lowercase) {
				GDesktopAppInfo *app_info = g_desktop_app_info_new_from_filename(directory_entry.path().c_str());
				icon_name = g_desktop_app_info_get_string(app_info, "Icon");
				break;
			}

			if (directory_entry.path().stem().has_extension()) {
				std::string app_name_ext = directory_entry.path().stem().extension().stem();
				std::string app_name_ext_lowercase = app_name_ext;

				std::transform(app_name_ext_lowercase.begin(), app_name_ext_lowercase.end(),
							   app_name_ext_lowercase.begin(), ::toupper);

				if (app_name_ext == "." + app_id || app_name_ext_lowercase == "." + app_id
					|| app_name_ext_lowercase == "." + app_id_lowercase) {
					GDesktopAppInfo *app_info = g_desktop_app_info_new_from_filename(directory_entry.path().c_str());
					icon_name = g_desktop_app_info_get_string(app_info, "Icon");
					break;
				}
			}
		}
	}

	if (icon_name.length() == 0) {
		log_warn("app_id is not matching aby .desktop file, setting default icon");
		icon_name = DEFAULT_APP_ICON;
	}

	return icon_name;
}
}