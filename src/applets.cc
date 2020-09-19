#include "applets.hh"
#include <filesystem>

#include "log.hh"

#include <dlfcn.h>

namespace wapanel::applets {
namespace fs = std::filesystem;

applets_container applets;

auto try_loading_symbol(void *dso_handle, const char *name) -> std::optional<void *> {
	void *symbol = dlsym(dso_handle, name);

	if (dlerror() != NULL) {
		dlclose(dso_handle);

		log_error("Encountered error while trying to load '%s' function", name);
		return {};
	}

	log_info("Successfully loaded '%s' function", name);

	return symbol;
}

auto try_loading_applet(const char *path) -> void {
	void *dso_handle;

	struct _papi_applet_info {
		const char *name;
		unsigned int version;
	};

	_papi_applet_info papi_applet_info;
	applets::applet_info internal_applet_info;

	// Define handy-dandy types for reinterpret_casting.
	typedef _papi_applet_info (*apl_wap_applet_info_t)(void);
	typedef GtkWidget *(*apl_wap_new_instance_t)(wap_t_applet_config);
	typedef void (*apl_wap_event_remove_instances_t)(void);
	typedef void (*apl_wap_event_exit_t)(void);

	apl_wap_applet_info_t wap_applet_info;

	// DSO loading stuff.
	if ((dso_handle = dlopen(path, RTLD_NOW)) == NULL) {
		log_error("Failed to open '%s' file", path);
		return;
	}

	log_info("successfully loaded '%s' file", path);

	// Retrieving applet info if possible.
	auto symbol = try_loading_symbol(dso_handle, "wap_applet_info");
	if (!symbol) return;

	wap_applet_info = reinterpret_cast<apl_wap_applet_info_t>(symbol.value());

	papi_applet_info = wap_applet_info();

	internal_applet_info.name = papi_applet_info.name;
	internal_applet_info.version = papi_applet_info.version;
	internal_applet_info.dso_handle = dso_handle;
	internal_applet_info.dso_path = path;

	// Binding functions from applet to internal info.

	// New instance function.
	symbol = try_loading_symbol(dso_handle, "wap_applet_new_instance");
	if (!symbol) return;
	internal_applet_info.new_instance = reinterpret_cast<apl_wap_new_instance_t>(symbol.value());

	// Remove instance function.
	symbol = try_loading_symbol(dso_handle, "wap_event_remove_instances");
	if (!symbol) return;
	internal_applet_info.event_remove_instances = reinterpret_cast<apl_wap_event_remove_instances_t>(symbol.value());

	// Exit event.
	symbol = try_loading_symbol(dso_handle, "wap_event_exit");
	if (!symbol) return;
	internal_applet_info.event_exit = reinterpret_cast<apl_wap_event_exit_t>(symbol.value());

	// Push applet info to all applets.
	applets::applets._applets[std::string(internal_applet_info.name)] = internal_applet_info;
}

auto search_for_applets() -> void {
	std::string applet_paths[] = APPLET_SEARCH_PATHS;

#ifndef RELEASE
	applet_paths[0] = "../lib/wapanel/applets";
#endif

	for (auto &&path : applet_paths) {
		if (!fs::exists(path)) continue;

		for (auto &&dir_entry : fs::directory_iterator(path)) {
			if (!dir_entry.is_regular_file()) continue;

			try_loading_applet(dir_entry.path().c_str());
		}
	}
}

auto remove_applets() -> void {
	for (auto &&[k, applet] : applets::applets._applets) {
		applet.event_exit();
		dlclose(applet.dso_handle);
	}

	applets::applets._applets.clear();
}

auto remove_existing_instances() -> void {
	for (auto &&[k, applet] : applets::applets._applets) {
		applet.event_remove_instances();
	}
}

auto applet_get_new_instance(std::string name, wap_t_applet_config applet_config) -> std::optional<GtkWidget *> {
	if (!applets::applets._applets.contains(name)) return {};

	return applets::applets._applets[name].new_instance(applet_config);
}

}