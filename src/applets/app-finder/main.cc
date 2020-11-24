#include "app-finder.hh"
#include <appletapi.h>
#include <gtk/gtk.h>
#include <vector>

std::vector<wapanel::applet::app_finder *> instances;

extern "C" {

// Called to get handy info about this applet.
wap_t_applet_info wap_applet_info() { return { .name = "app-finder", .version = 1 }; }

// Called when some panel need new instance of your applet.
GtkWidget *wap_applet_new_instance(wap_t_applet_config applet_config) {
	wapanel::applet::app_finder *af = new wapanel::applet::app_finder(applet_config, instances.size());

	instances.push_back(af);

	return af->get_widget();
}

// Called when requested to remove all existing instances. GtkWidget should be disposed by panel.
void wap_event_remove_instances() {
	for (auto &&cl : instances) {
		delete cl;
	}

	instances.clear();
}

// Called when panel exits.
void wap_event_exit() {}
}