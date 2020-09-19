#pragma once
#include <vector>
#include <string>

#include <toml11/toml.hpp>

#define CONFIG_SEARCH_PATHS {"./wapanel.toml", "~/.config/wapanel.toml"}

namespace wapanel::conf {

// Panel stuff.
enum class panel_position {
	TOP,
	BOTTOM
};

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

// This functions reads config from
// first found path and places data in wapanel::conf::config.
// Returns true if found any error.
auto read_config() -> bool;

}