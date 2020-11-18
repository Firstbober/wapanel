#include <appletapi.h>
#include <gtk/gtk.h>
#include <vector>
#include <thread>
#include "backend.hh"
#include "backends/pulseaudio.hh"

// std::vector<*> instances;

wapanel::applet::backend* backend;
std::thread runThread;

extern "C" {

// Called to get handy info about this applet.
wap_t_applet_info wap_applet_info() { return { .name = "volume-control", .version = 1 }; }

// Called when some panel need new instance of your applet.
GtkWidget *wap_applet_new_instance(wap_t_applet_config applet_config) {
	// x *vc = new x(applet_config);
	backend = new wapanel::applet::backends::pulseaudio();
	backend->initialize();

	runThread = std::thread(&wapanel::applet::backend::run, backend);

	// instances.push_back(vc);
	// return vc->get_widget();

	GtkLabel *label = GTK_LABEL(gtk_label_new("|| volume-control ||"));

	return GTK_WIDGET(label);
}

// Called when requested to remove all existing instances. GtkWidget should be disposed by panel.
void wap_event_remove_instances() {
	/*
	for (auto &&vc : instances) {
		delete vc;
	}

	instances.clear();
	*/
}

// Called when panel exits.
void wap_event_exit() {
	delete backend;
	runThread.join();
}
}