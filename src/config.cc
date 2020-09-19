#include "config.hh"
#include <filesystem>
#include <vector>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.hh"
#include "utils.hh"

#include <toml11/toml.hpp>

namespace wapanel::conf {
namespace fs = std::filesystem;

global_config config;

auto read_config() -> bool {
	// Config finding code.
	const char *home_dir;
	bool found_config = false;
	std::string found_config_path;

	conf::config = {};

	// Check if enviroment variable 'HOME' exists
	// then use it as home directory and if this env-var
	// doesn't exist then use user database entry and extract home dir.
	if ((home_dir = getenv("HOME")) == NULL) { home_dir = getpwuid(getuid())->pw_dir; }

	std::string config_paths[] = CONFIG_SEARCH_PATHS;
	for (auto &&config_path : config_paths) {
		// If config path contains '~' as first character
		// then replace it with home dir.
		if (config_path[0] == '~') config_path.replace(0, 1, std::string(home_dir));

		log_info("Checking if config file '%s' exists", config_path.c_str());

		if (fs::exists(config_path)) {
			log_info("Found file");

			found_config_path = config_path;
			found_config = true;
			break;
		}

		log_info("File not found");
	}

	if (!found_config) {
		log_error("Didn't found any config file!");
		return true;
	}

	config.used_config_file = found_config_path;

	// Config reading code.
	const auto tm_data = toml::parse(found_config_path);
	const auto panels = toml::find<std::vector<toml::value>>(tm_data, "panel");

	// Iterate over panel entries in toml.
	for (auto &&tm_panel : panels) {
		panel_config pn_config;

		const auto str_position = toml::find_or(tm_panel, "position", "bottom");

		pn_config.position = str_position == "top" ? panel_position::TOP : panel_position::BOTTOM;
		pn_config.height = toml::find_or(tm_panel, "height", 32);

		// Read and configure applets in panel.
		if (tm_panel.contains("applet")) {
			const auto applets = toml::find<std::vector<toml::value>>(tm_panel, "applet");

			for (auto &&tm_applet : applets) {
				if (!tm_applet.contains("name") || toml::find_or(tm_applet, "name", "").length() == 0) {
					spawn_gtk_error("Tried to load applet without name");
					log_error("Tried to load applet without name");

					continue;
				}

				pn_config.applets_config.push_back(
					std::make_pair(toml::find<std::string>(tm_applet, "name"), tm_applet));
			}
		}

		log_info("Loaded configuration of panel {%s, %d}", str_position.c_str(), pn_config.height);

		config.panels_conf.push_back(pn_config);
	}

	return false;
}
}