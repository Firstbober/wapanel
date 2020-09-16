#include "config.hh"
#include <filesystem>
#include <vector>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.hh"

#include <toml11/toml.hpp>

namespace wapanel::conf {
namespace fs = std::filesystem;

global_config config;

auto free_wap_t_config_variable(_wap_t_config_variable variable) -> void {
	switch (variable.type) {
	case WAP_CONF_VAR_TYPE_ARRAY:
		for (size_t i = 0; i < variable.content.array._size; i++) {
			free_wap_t_config_variable(variable.content.array._content[i]);

			free(variable.content.array._content);
			variable.content.array._content = NULL;
			variable.content.array._size = 0;
		}

		break;

	case WAP_CONF_VAR_TYPE_TABLE:
		for (size_t i = 0; i < variable.content.table._size; i++) {
			free_wap_t_config_variable(variable.content.table._content[i]);

			free(variable.content.table._content);
			variable.content.table._content = NULL;
			variable.content.table._size = 0;
			variable.content.table.name = NULL;
		}

		break;

	default:
		break;
	}
}

auto free_applet_config(wap_t_applet_config applet_config) -> void {
	log_info("freeing applet_config");
	for (size_t i = 0; i < applet_config._size; i++) {
		free_wap_t_config_variable(applet_config.root[i]);

		free(applet_config.root);
		applet_config._size = 0;
	}
}

// Another resolver with identical code was needed to fullfill vector requirements
// Please don't touch unless you know what you doing !_!
auto recurse_table(toml::table &table) -> std::vector<_wap_t_config_variable>;
auto resolve_array(toml::array &array) -> std::vector<_wap_t_config_variable> {
	log_info("resolving array...");
	std::vector<_wap_t_config_variable> _content;

	unsigned int current_index = 0;

	for (auto &&value : array) {
		_wap_t_config_variable variable = {};
		variable.key.number = current_index;

		// Match type and put good value into variable
		switch (value.type()) {
		case toml::value_t::string:
			variable.type == WAP_CONF_VAR_TYPE_STRING;
			variable.content.value.string = value.as_string().str.c_str();
			break;

		case toml::value_t::integer:
			variable.type == WAP_CONF_VAR_TYPE_INTEGER;
			variable.content.value.integer = value.as_integer();
			break;

		case toml::value_t::floating:
			variable.type == WAP_CONF_VAR_TYPE_FLOAT;
			variable.content.value.floating = value.as_floating();
			break;

		case toml::value_t::boolean:
			variable.type == WAP_CONF_VAR_TYPE_BOOLEAN;
			variable.content.value.boolean = value.as_boolean();
			break;

		case toml::value_t::array: {
			variable.type == WAP_CONF_VAR_TYPE_ARRAY;
			toml::array tm_array = value.as_array();

			std::vector<_wap_t_config_variable> var_list = resolve_array(value.as_array());
			if (var_list.size() > 0) {
				variable.content.array._content = reinterpret_cast<_wap_t_config_variable *>(
					malloc(var_list.size() * sizeof(_wap_t_config_variable)));

				variable.content.array._size = var_list.size();
			}

			size_t num_index = 0;
			for (auto &&_var : var_list) {
				variable.content.array._content[num_index] = _var;
				num_index++;
			}

			break;
		}

		case toml::value_t::table: {
			variable.type == WAP_CONF_VAR_TYPE_TABLE;
			variable.content.table.name = std::to_string(current_index).c_str();

			std::vector<_wap_t_config_variable> var_list = recurse_table(value.as_table());
			if (var_list.size() > 0) {
				variable.content.table._content = reinterpret_cast<_wap_t_config_variable *>(
					malloc(var_list.size() * sizeof(_wap_t_config_variable)));

				variable.content.table._size = var_list.size();
			}

			size_t num_index = 0;
			for (auto &&_var : var_list) {
				variable.content.table._content[num_index] = _var;
				num_index++;
			}

			break;
		}

		default:
			break;
		}

		current_index++;
		_content.push_back(variable);
	}

	return _content;
}
auto recurse_table(toml::table &table) -> std::vector<_wap_t_config_variable> {
	log_info("reccurenting table...");
	std::vector<_wap_t_config_variable> _content;

	for (auto &&val_pair : table) {
		_wap_t_config_variable variable = {};
		variable.key.string = val_pair.first.c_str();

		// Match type and put good value into variable
		switch (val_pair.second.type()) {
		case toml::value_t::string:
			variable.type == WAP_CONF_VAR_TYPE_STRING;
			variable.content.value.string = val_pair.second.as_string().str.c_str();
			break;

		case toml::value_t::integer:
			variable.type == WAP_CONF_VAR_TYPE_INTEGER;
			variable.content.value.integer = val_pair.second.as_integer();
			break;

		case toml::value_t::floating:
			variable.type == WAP_CONF_VAR_TYPE_FLOAT;
			variable.content.value.floating = val_pair.second.as_floating();
			break;

		case toml::value_t::boolean:
			variable.type == WAP_CONF_VAR_TYPE_BOOLEAN;
			variable.content.value.boolean = val_pair.second.as_boolean();
			break;

		case toml::value_t::array: {
			variable.type == WAP_CONF_VAR_TYPE_ARRAY;
			toml::array tm_array = val_pair.second.as_array();

			std::vector<_wap_t_config_variable> var_list = resolve_array(val_pair.second.as_array());
			if (var_list.size() > 0) {
				variable.content.array._content = reinterpret_cast<_wap_t_config_variable *>(
					malloc(var_list.size() * sizeof(_wap_t_config_variable)));

				variable.content.array._size = var_list.size();
			}

			size_t num_index = 0;
			for (auto &&_var : var_list) {
				variable.content.array._content[num_index] = _var;
				num_index++;
			}

			break;
		}

		case toml::value_t::table: {
			variable.type == WAP_CONF_VAR_TYPE_TABLE;
			variable.content.table.name = val_pair.first.c_str();

			std::vector<_wap_t_config_variable> var_list = recurse_table(val_pair.second.as_table());
			if (var_list.size() > 0) {
				variable.content.table._content = reinterpret_cast<_wap_t_config_variable *>(
					malloc(var_list.size() * sizeof(_wap_t_config_variable)));

				variable.content.table._size = var_list.size();
			}

			size_t num_index = 0;
			for (auto &&_var : var_list) {
				variable.content.table._content[num_index] = _var;
				num_index++;
			}

			break;
		}

		default:
			break;
		}

		_content.push_back(variable);
	}

	return _content;
}

auto insert_applet_config_to_panel(panel_config &pn_config, toml::value tm_applet) -> void {
	wap_t_applet_config applet_config;
	toml::table tm_applet_table;

	tm_applet_table = tm_applet.as_table();

	std::vector<_wap_t_config_variable> var_list = recurse_table(tm_applet_table);
	if (var_list.size() > 0) {
		applet_config.root
			= reinterpret_cast<_wap_t_config_variable *>(malloc(var_list.size() * sizeof(_wap_t_config_variable)));

		applet_config._size = var_list.size();
	}

	size_t num_index = 0;
	for (auto &&variable : var_list) {
		applet_config.root[num_index] = variable;
		num_index++;
	}

	pn_config.applets_config.push_back(std::make_pair(toml::find<std::string>(tm_applet, "name"), applet_config));
	log_info("inserted applet_config with applet '%s' to panel_config",
			 toml::find<std::string>(tm_applet, "name").c_str());
}

auto read_config() -> bool {
	// Config finding code
	const char *home_dir;
	bool found_config = false;
	std::string found_config_path;

	{
		for (auto &&panel_config : conf::config.panels_conf) {
			for (auto &&applet_config : panel_config.applets_config) {
				free_applet_config(applet_config.second);
			}
		}

		conf::config = {};
	}

	// Check if enviroment variable 'HOME' exists
	// then use it as home directory and if this env-var
	// doesn't exist then use user database entry and extract home dir.
	if ((home_dir = getenv("HOME")) == NULL) { home_dir = getpwuid(getuid())->pw_dir; }

	std::string config_paths[] = CONFIG_SEARCH_PATHS;
	for (auto &&config_path : config_paths) {
		// If config path contains '~' as first character
		// then replace it with home dir
		if (config_path[0] == '~') config_path.replace(0, 1, std::string(home_dir));

		log_info("checking if config file '%s' exists", config_path.c_str());

		if (fs::exists(config_path)) {
			log_info("found file");

			found_config_path = config_path;
			found_config = true;
			break;
		}

		log_info("file not found");
	}

	if (!found_config) {
		log_error("didn't found any config file!");
		return true;
	}

	config.used_config_file = found_config_path;

	// Config reading code
	const auto tm_data = toml::parse(found_config_path);
	const auto panels = toml::find<std::vector<toml::value>>(tm_data, "panel");

	// Iterate over panel entries in toml
	for (auto &&tm_panel : panels) {
		panel_config pn_config;

		const auto str_position = toml::find_or(tm_panel, "position", "bottom");

		pn_config.position = str_position == "top" ? panel_position::TOP : panel_position::BOTTOM;
		pn_config.height = toml::find_or(tm_panel, "height", 32);

		// Read and configure applets in panel
		if (tm_panel.contains("applet")) {
			const auto applets = toml::find<std::vector<toml::value>>(tm_panel, "applet");

			for (auto &&tm_applet : applets) {
				if (!tm_applet.contains("name")) { log_error("tried to load applet without name"); }

				insert_applet_config_to_panel(pn_config, tm_applet);
			}
		}

		log_info("loaded configuration of panel {%s, %d}", str_position.c_str(), pn_config.height);

		config.panels_conf.push_back(pn_config);
	}

	return false;
}
}