#include "clock.hh"
#include <appletapi.h>
#include <gtk/gtk.h>
#include <vector>

std::vector<wapanel::applet::clock *> instances;

extern "C" {

// Called to get handy info about this applet.
wap_t_applet_info wap_applet_info() { return { .name = "clock", .version = 1 }; }

// Called when some panel need new instance of your applet.
GtkWidget *wap_applet_new_instance(wap_t_applet_config applet_config) {
	wapanel::applet::clock *cl = new wapanel::applet::clock(applet_config);

	instances.push_back(cl);

	return cl->get_widget();
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