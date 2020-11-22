#include <gio/gio.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <stdio.h>
#include <vector>

#include "../config.hh"
#include "applets.hh"
#include "log.hh"
#include "panel.hh"
#include "utils.hh"

// Basic info
std::vector<wapanel::panel *> panels;
GtkApplication *app;
GFileMonitor *config_monitor;
// Basic info

// Command line things

auto static cmd_list_available_applets() -> void {
	wapanel::applets::search_for_applets();

	printf("==========================\n\n");
	printf("Available applets (by name usable in config):\n\n");

	for (auto &&applet : wapanel::applets::applets._applets) {
		printf("\t%s\n", applet.second.name);
	}

	printf("\n==========================\n");

	exit(0);
}

static GOptionEntry cmd_entries[]
	= { { "list-available-applets", 'l', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
		  (GOptionArgFunc *)(cmd_list_available_applets), "Shows all available applets to use.", NULL },
		{ NULL } };

// App and config stuff

auto static app_activate(GtkApplication *_app) -> void;
auto static config_changed(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type,
						   gpointer user_data) -> void {
	if (event_type != G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT) { return; }

	log_info("====== Config file update ======");

	// Exit if error is found.
	if (wapanel::conf::read_config()) {
		wapanel::spawn_gtk_error("Found some error while trying to read config file. Exiting.");
		exit(1);
	}

	wapanel::applets::remove_existing_instances();

	// Check if any panels is removed or added to the config.
	if (wapanel::conf::config.panels_conf.size() != panels.size()) {
		// Remove existing panels.
		for (auto &&panel : panels) {
			gtk_application_remove_window(app, panel->get_gtk_window());
			delete panel;
		}

		panels.clear();

		// Init all again panels.
		app_activate(app);
		return;
	}

	// Reconfigure panels.
	for (auto &&panel : panels) {
		panel->configure();
	}
}

auto static app_startup(GtkApplication *_app) -> void {
	// Exit if error is found.
	if (wapanel::conf::read_config()) {
		GtkWindow *dialog_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_POPUP));
		GtkMessageDialog *message_dialog = GTK_MESSAGE_DIALOG(
			gtk_message_dialog_new(dialog_window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
								   GTK_BUTTONS_OK_CANCEL, "No configuration file was found. Create a new one?"));

		int result = gtk_dialog_run(GTK_DIALOG(message_dialog));
		gtk_widget_destroy(GTK_WIDGET(dialog_window));

		if (result == GTK_RESPONSE_OK) {
			if (!std::filesystem::exists(MAIN_CONFIG_DIR)) std::filesystem::create_directory(MAIN_CONFIG_DIR);

			std::filesystem::copy_file(std::string(DATA_DIR) + std::string("/wapanel.toml"), MAIN_CONFIG_FILE);
			app_startup(_app);
		} else {
			exit(1);
		}
	}

	// Monitor for changes in config.
	config_monitor = g_file_monitor_file(g_file_new_for_path(wapanel::conf::config.used_config_file.c_str()),
										 G_FILE_MONITOR_NONE, NULL, NULL);
	g_signal_connect(config_monitor, "changed", G_CALLBACK(config_changed), &wapanel::conf::config);

	// Searches various paths for applets.
	wapanel::applets::search_for_applets();
}

auto static app_activate(GtkApplication *_app) -> void {
	for (size_t i = 0; i < wapanel::conf::config.panels_conf.size(); i++) {
		wapanel::panel *panel = new wapanel::panel(i);

		gtk_application_add_window(_app, panel->get_gtk_window());

		log_info("Displaying panel %zu", i);

		panels.push_back(panel);
	}
}

auto static app_shutdown(GtkApplication *_app) -> void { wapanel::applets::remove_applets(); }

// Signal handling

auto static signal_interrupt_callback(int sig) {
	fprintf(stderr, "\n");

	wapanel::applets::remove_existing_instances();
	wapanel::applets::remove_applets();

	exit(sig);
}

// Main function

auto main(int argc, char **argv) -> int {
	int status;

	log_info("Started wapanel version %s", VERSION_STR);

	// Connect SIGINT to callback
	signal(SIGINT, signal_interrupt_callback);

	// Create GTK Application.
	app = gtk_application_new("com.firstbober.wapanel", G_APPLICATION_FLAGS_NONE);

	g_application_add_main_option_entries(G_APPLICATION(app), cmd_entries);

	g_signal_connect(app, "startup", G_CALLBACK(app_startup), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
	g_signal_connect(app, "shutdown", G_CALLBACK(app_shutdown), NULL);

	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	// Remove panels on exit.
	for (auto &&panel : panels) {
		delete panel;
	}

	return status;
}