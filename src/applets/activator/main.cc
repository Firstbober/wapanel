#include "activator.hh"
#include <appletapi.h>
#include <vector>

std::vector<wapanel::applet::activator *> instances;

extern "C" {

// Called to get handy info about this applet.
wap_t_applet_info wap_applet_info() { return { .name = "activator", .version = 1 }; }

// Called when some panel need new instance of your applet.
GtkWidget *wap_applet_new_instance(wap_t_applet_config applet_config) {
	wapanel::applet::activator *ac = new wapanel::applet::activator(applet_config, instances.size());
	instances.push_back(ac);

	return ac->get_widget();
}

// Called when requested to remove all existing instances. GtkWidget should be disposed by panel.
void wap_event_remove_instances() {
	for (auto &&instance : instances) {
		delete instance;
	}

	instances.clear();
}

// Called when panel exits.
void wap_event_exit() {}
}