#include "task_switcher.hh"
#include "wl_toplevel_manager.hh"
#include <appletapi.h>
#include <gtk/gtk.h>
#include <string>
#include <vector>

std::vector<wapanel::applet::task_switcher *> instances;

extern "C" {

// Called to get handy info about this applet.
wap_t_applet_info wap_applet_info() { return { .name = "task-switcher", .version = 1 }; }

// Called when some panel need new instance of your applet.
GtkWidget *wap_applet_new_instance(wap_t_applet_config applet_config) {
	wapanel::applet::task_switcher *ts = new wapanel::applet::task_switcher(applet_config);
	instances.push_back(ts);

	return ts->get_widget();
}

// Called when requested to remove all existing instances. GtkWidget should be disposed by panel.
void wap_event_remove_instances() {
	for (auto &&cl : instances) {
		delete cl;
	}

	instances.clear();
	wapanel::applet::wl::toplevel_manager::get().clean();
}

// Called when panel exits.
void wap_event_exit() {}
}