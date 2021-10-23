#pragma once
#include <gtk/gtk.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../config_data.hh"
#include "lib/appletapi.h"

#define APPLET_SEARCH_PATHS                                                                                            \
	{ "", "./wapanel/applets", LIB_DIR "/wapanel/applets", "~/.local/share/wapanel/applets" }

namespace wapanel::applets {

// Specific applet data.
struct applet_info {
	const char *name;
	unsigned int version;

	// Path to dynamically shared object file.
	const char *dso_path;

	// DSO handle.
	void *dso_handle;

	// Easy-to-access functions.
	GtkWidget *(*new_instance)(wap_t_applet_config);
	void (*event_remove_instances)(void);
	void (*event_exit)(void);
};

// Struct for storing general informations about applets.
struct applets_container {
	std::unordered_map<std::string, applet_info> _applets;
};

// Global variable for easy access.
extern applets_container applets;

// Searches for applets then saves info about them in applet_info.
auto search_for_applets() -> void;

// Safetly remove all applets.
auto remove_applets() -> void;

// Removes all existing instances of applet.
auto remove_existing_instances() -> void;

// Gets gtk widget for use in panel from applet.
auto applet_get_new_instance(std::string name, wap_t_applet_config applet_config) -> std::optional<GtkWidget *>;

}