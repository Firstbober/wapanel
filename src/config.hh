#pragma once
#include <string>
#include <toml11/toml.hpp>
#include <vector>

namespace wapanel::conf {

// Panel stuff.
enum class panel_position { TOP, BOTTOM };

struct panel_config {
	panel_position position;
	unsigned int height;

	std::vector<std::pair<std::string, toml::value>> applets_config;
};

// General stuff.
struct global_config {
	std::vector<panel_config> panels_conf;
	std::string used_config_file;
};

extern global_config config;

extern bool is_there_last_config;
extern toml::value last_working_config;

// This functions reads config from
// first found path and places data in wapanel::conf::config.
// Returns true if found any error.
auto read_config() -> bool;

}