#include "wap_t_convert.hh"
#include "log.hh"
#include "src/lib/appletapi.h"
#include "toml11/toml/types.hpp"

namespace wapanel::conv {

// Another resolver with identical code was needed to fullfill vector requirements
// Please don't touch unless you know what you doing !_!
auto recurse_table(toml::table &table) -> std::vector<_wap_t_config_variable>;
auto resolve_array(toml::array &array) -> std::vector<_wap_t_config_variable> {
	log_info("Resolving array...");
	std::vector<_wap_t_config_variable> _content;

	unsigned int current_index = 0;

	for (auto &&value : array) {
		_wap_t_config_variable variable = {};
		variable.key.number = current_index;
		variable.key.string = NULL;

		// Match type and put good value into variable
		switch (value.type()) {
		case toml::value_t::string: {
			variable.type = WAP_CONF_VAR_TYPE_STRING;

			char *temp_str = (char *)malloc(value.as_string().str.length() + 1);
			memcpy(temp_str, value.as_string().str.c_str(), value.as_string().str.length() + 1);

			variable.content.value.string = temp_str;
			break;
		}

		case toml::value_t::integer:
			variable.type = WAP_CONF_VAR_TYPE_INTEGER;
			variable.content.value.integer = value.as_integer();
			break;

		case toml::value_t::floating:
			variable.type = WAP_CONF_VAR_TYPE_FLOAT;
			variable.content.value.floating = value.as_floating();
			break;

		case toml::value_t::boolean:
			variable.type = WAP_CONF_VAR_TYPE_BOOLEAN;
			variable.content.value.boolean = value.as_boolean();
			break;

		case toml::value_t::array: {
			variable.type = WAP_CONF_VAR_TYPE_ARRAY;
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
			variable.type = WAP_CONF_VAR_TYPE_TABLE;

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
	log_info("Reccurenting table...");
	std::vector<_wap_t_config_variable> _content;

	for (auto &&val_pair : table) {
		_wap_t_config_variable variable = {};

		char *temp_str = (char *)malloc(val_pair.first.length() + 1);
		memcpy(temp_str, val_pair.first.c_str(), val_pair.first.length() + 1);

		variable.key.string = temp_str;

		// Match type and put good value into variable
		switch (val_pair.second.type()) {
		case toml::value_t::string: {
			variable.type = WAP_CONF_VAR_TYPE_STRING;

			temp_str = (char *)malloc(val_pair.second.as_string().str.length() + 1);
			memcpy(temp_str, val_pair.second.as_string().str.c_str(), val_pair.second.as_string().str.length() + 1);

			variable.content.value.string = temp_str;
			break;
		}

		case toml::value_t::integer:
			variable.type = WAP_CONF_VAR_TYPE_INTEGER;
			variable.content.value.integer = val_pair.second.as_integer();
			break;

		case toml::value_t::floating:
			variable.type = WAP_CONF_VAR_TYPE_FLOAT;
			variable.content.value.floating = val_pair.second.as_floating();
			break;

		case toml::value_t::boolean:
			variable.type = WAP_CONF_VAR_TYPE_BOOLEAN;
			variable.content.value.boolean = val_pair.second.as_boolean();
			break;

		case toml::value_t::array: {
			variable.type = WAP_CONF_VAR_TYPE_ARRAY;
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
			variable.type = WAP_CONF_VAR_TYPE_TABLE;

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

auto convert_toml_to_wap_t_config_variable(toml::value &value, unsigned int panel_height) -> _wap_t_config_variable * {
	_wap_t_config_variable *config_variable
		= reinterpret_cast<_wap_t_config_variable *>(malloc(sizeof(_wap_t_config_variable)));
	toml::table tm_table;

	tm_table = value.as_table();
	tm_table["__panel_height"] = toml::integer(panel_height);

	config_variable->type = WAP_CONF_VAR_TYPE_TABLE;

	std::vector<_wap_t_config_variable> var_list = recurse_table(tm_table);
	if (var_list.size() > 0) {
		config_variable->content.table._content
			= reinterpret_cast<_wap_t_config_variable *>(malloc(var_list.size() * sizeof(_wap_t_config_variable)));

		config_variable->content.table._size = var_list.size();
	}

	size_t num_index = 0;
	for (auto &&variable : var_list) {
		config_variable->content.table._content[num_index] = variable;
		num_index++;
	}

	return config_variable;
}

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
		}

		break;

	case WAP_CONF_VAR_TYPE_STRING:
		free((void *)variable.content.value.string);
		variable.content.value.string = NULL;

		break;

	default:
		break;
	}

	if (variable.key.string != NULL) { free((void *)variable.key.string); }
}

}
