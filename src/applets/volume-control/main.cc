#include "../../log.hh"
#include "../icon_cache.hh"
#include "backend.hh"
#include "backends/pulseaudio.hh"
#include "volume-control.hh"
#include <appletapi.h>
#include <gtk/gtk.h>
#include <thread>
#include <vector>

std::vector<wapanel::applet::volume_control *> instances;
wapanel::applet::backend *backend = NULL;
std::thread backend_thread;

extern "C" {

// Called to get handy info about this applet.
wap_t_applet_info wap_applet_info() { return { .name = "volume-control", .version = 1 }; }

// Called when some panel need new instance of your applet.
GtkWidget *wap_applet_new_instance(wap_t_applet_config applet_config) {
	if (backend == NULL) {
		backend = new wapanel::applet::backends::pulseaudio();

		if (backend->initialize()) {
			backend_thread = std::thread(&wapanel::applet::backend::run, backend);
		} else {
			log_error("Failed to initialize backend!");
			return NULL;
		}
	}

	if (backend != NULL) {
		wapanel::applet::volume_control *vc
			= new wapanel::applet::volume_control(applet_config, backend, instances.size());
		instances.push_back(vc);

		return GTK_WIDGET(vc->get_widget());
	}

	return NULL;
}

// Called when requested to remove all existing instances. GtkWidget should be disposed by panel.
void wap_event_remove_instances() {
	for (auto &&vc : instances) {
		delete vc;
	}

	instances.clear();
}

// Called when panel exits.
void wap_event_exit() {
	if (backend != NULL) delete backend;

	wapanel::applet::utils::ic::clean();

	if (backend_thread.joinable()) backend_thread.join();
}
}