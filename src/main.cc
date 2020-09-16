#include <gio/gio.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <vector>

#include "applets.hh"
#include "config.hh"
#include "log.hh"
#include "panel.hh"

// Basic info
std::vector<wapanel::panel *> panels;
GtkApplication *app;
// Basic info

static void app_activate(GtkApplication *_app) {
	for (size_t i = 0; i < wapanel::conf::config.panels_conf.size(); i++) {
		wapanel::panel *panel = new wapanel::panel(i);

		gtk_application_add_window(app, panel->get_gtk_window());

		log_info("displaying panel %i", i);

		panels.push_back(panel);
	}
}

auto static app_shutdown(GtkApplication *_app) -> void { wapanel::applets::remove_applets(); }

auto static config_changed(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type,
						   gpointer user_data) -> void {
	if (event_type != G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT) { return; }

	log_info("====== config file update ======");

	// Exit if error is found
	if (wapanel::conf::read_config()) exit(1);

	wapanel::applets::remove_existing_instances();

	// Check if any panels is removed or added to the config
	if (wapanel::conf::config.panels_conf.size() != panels.size()) {
		// Remove existing panels
		for (auto &&panel : panels) {
			gtk_application_remove_window(app, panel->get_gtk_window());
			delete panel;
		}

		panels.clear();

		// Init all again panels
		app_activate(app);
		return;
	}

	// Reconfigure panels
	for (auto &&panel : panels) {
		panel->configure();
	}
}

auto main(int argc, char **argv) -> int {
	GFileMonitor *config_monitor;
	int status;

	log_info("started");

	// Exit if error is found
	if (wapanel::conf::read_config()) return 1;

	// Monitor for changes in config
	config_monitor = g_file_monitor_file(g_file_new_for_path(wapanel::conf::config.used_config_file.c_str()),
										 G_FILE_MONITOR_NONE, NULL, NULL);
	g_signal_connect(config_monitor, "changed", G_CALLBACK(config_changed), &wapanel::conf::config);

	// Searches various paths for applets
	wapanel::applets::search_for_applets();

	// Create GTK Application
	app = gtk_application_new("com.firstbober.wapanel", G_APPLICATION_FLAGS_NONE);

	g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
	g_signal_connect(app, "shutdown", G_CALLBACK(app_shutdown), NULL);

	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	// Remove panels on exit
	for (auto &&panel : panels) {
		delete panel;
	}

	return status;
}