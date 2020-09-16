#include "panel.hh"
#include "applets.hh"
#include "log.hh"

#include <gtk-layer-shell.h>
#include <gtk/gtk.h>

namespace wapanel {

panel::panel(unsigned int id, conf::global_config *config)
	: m_id(id) {
	m_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

	// Panel's window settings
	gtk_window_set_title(m_window, "wapanel-panel");
	gtk_window_set_decorated(m_window, false);
	gtk_window_set_resizable(m_window, false);

	// GTK Layer Shell configuration
	gtk_layer_init_for_window(m_window);

	gtk_layer_set_layer(m_window, GTK_LAYER_SHELL_LAYER_TOP);

	// Repell other windows
	gtk_layer_auto_exclusive_zone_enable(m_window);

	// Resize window to maximum possible width
	gtk_layer_set_anchor(m_window, GTK_LAYER_SHELL_EDGE_LEFT, true);
	gtk_layer_set_anchor(m_window, GTK_LAYER_SHELL_EDGE_RIGHT, true);

	// Create gtk box container for applets
	m_appletbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
	gtk_container_add(GTK_CONTAINER(m_window), GTK_WIDGET(m_appletbox));

	// General configuration
	this->configure(config);

	// Show all in window - moved to configure for convince purposes
}
panel::~panel() { gtk_widget_destroy(GTK_WIDGET(m_window)); }

auto panel::get_gtk_window() -> GtkWindow * { return m_window; }

auto panel::configure(conf::global_config *config) -> void {
	log_info("configuring panel %i", m_id);

	// Set size of window
	gtk_widget_set_size_request(GTK_WIDGET(m_window), 0, conf::config.panels_conf[m_id].height);

	// Set position on desktop
	if (conf::config.panels_conf[m_id].position == conf::panel_position::TOP) {
		gtk_layer_set_anchor(m_window, GTK_LAYER_SHELL_EDGE_TOP, true);
		gtk_layer_set_anchor(m_window, GTK_LAYER_SHELL_EDGE_BOTTOM, false);
	} else {
		gtk_layer_set_anchor(m_window, GTK_LAYER_SHELL_EDGE_TOP, false);
		gtk_layer_set_anchor(m_window, GTK_LAYER_SHELL_EDGE_BOTTOM, true);
	}

	// Make instances of configured applets
	auto applets_config = config->panels_conf[m_id].applets_config; // conf::config.panels_conf[m_id].applets_config;
	auto appletbox_children = gtk_container_get_children(GTK_CONTAINER(m_appletbox));

	g_list_foreach(appletbox_children, (GFunc)(+[](GtkWidget *widget, GtkWidget *appletbox) {
					   gtk_container_remove(GTK_CONTAINER(appletbox), widget);
				   }),
				   m_appletbox);
	log_info("removed last widgets");

	for (auto &&applet_config : applets_config) {
		auto applet_widget = applets::applet_get_new_instance(applet_config.first, &applet_config.second);

		log_error("aa: %s", applet_config.second.root[0].content.value.string);

		if (applet_widget) {
			gtk_container_add(GTK_CONTAINER(m_appletbox), applet_widget.value());
			log_info("added instance of applet '%s' to panel", applet_config.first.c_str());
		}
	}

	// Show all widgets in window
	gtk_widget_show_all(GTK_WIDGET(m_window));
}

}