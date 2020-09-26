#include "appletapi.h"
#include "../log.hh"
#include <string.h>

// Error handling stuff.

wap_t_error_type _wap_errno = -1;

bool wapi_error() { return _wap_errno != -1; }

void wapi_error_print() {
	switch (_wap_errno) {
	case WAP_ERR_VARIABLE_IS_NOT_TABLE:
		log_error("Specified variable is not a table.");
		break;

	case WAP_ERR_VARIABLE_IS_NOT_ARRAY:
		log_error("Specified variable is not an array.");
		break;

	case WAP_ERR_CANNOT_FIND:
		log_error("Cannot find specified key/index in table/array.");
		break;

	case WAP_ERR_INVALID_VAR_TYPE:
		log_error("Passed variable with invalid type to convert function.");

	default:
		log_error("Unknown error or absence of it.");
		break;
	}

	_wap_errno = -1;
}

// Variable stuff.

// Existance check.

bool wapi_key_exists(_wap_t_config_variable *table, const char *key) {
	_wap_errno = -1;
	if (table->type != WAP_CONF_VAR_TYPE_TABLE) {
		_wap_errno = WAP_ERR_VARIABLE_IS_NOT_TABLE;
		return false;
	}

	_wap_t_config_variable var;
	for (size_t i = 0; i < table->content.table._size; i++) {
		var = table->content.table._content[i];
		if (!strcmp(key, var.key.string)) { return true; }
	}

	return false;
}

bool wapi_index_exists(_wap_t_config_variable *array, size_t index) {
	_wap_errno = -1;
	if (array->type != WAP_CONF_VAR_TYPE_ARRAY) {
		_wap_errno = WAP_ERR_VARIABLE_IS_NOT_ARRAY;
		return false;
	}

	for (size_t i = 0; i < array->content.array._size; i++) {
		if (index == i) return true;
	}

	return false;
}

// Extracting.

_wap_t_config_variable *wapi_get_var_from_table(_wap_t_config_variable *table, const char *key) {
	_wap_errno = -1;
	if (table->type != WAP_CONF_VAR_TYPE_TABLE) {
		_wap_errno = WAP_ERR_VARIABLE_IS_NOT_TABLE;
		return NULL;
	}

	_wap_t_config_variable var;
	for (size_t i = 0; i < table->content.table._size; i++) {
		var = table->content.table._content[i];
		if (!strcmp(key, var.key.string)) return &table->content.table._content[i];
	}

	_wap_errno = WAP_ERR_CANNOT_FIND;
	return NULL;
}

_wap_t_config_variable *wapi_get_var_from_array(_wap_t_config_variable *array, size_t index) {
	_wap_errno = -1;
	if (array->type != WAP_CONF_VAR_TYPE_ARRAY) {
		_wap_errno = WAP_ERR_VARIABLE_IS_NOT_ARRAY;
		return NULL;
	}

	for (size_t i = 0; i < array->content.array._size; i++) {
		if (index == i) return &array->content.array._content[i];
	}

	_wap_errno = WAP_ERR_CANNOT_FIND;
	return NULL;
}

// Converting variable to type.

const char *wapi_var_as_string(_wap_t_config_variable *var) {
	_wap_errno = -1;
	if (var->type != WAP_CONF_VAR_TYPE_STRING) {
		_wap_errno = WAP_ERR_INVALID_VAR_TYPE;
		return NULL;
	}

	return var->content.value.string;
}

int64_t wapi_var_as_integer(_wap_t_config_variable *var) {
	_wap_errno = -1;
	if (var->type != WAP_CONF_VAR_TYPE_INTEGER) {
		_wap_errno = WAP_ERR_INVALID_VAR_TYPE;
		return -1;
	}

	return var->content.value.integer;
}

double wapi_var_as_floating(_wap_t_config_variable *var) {
	_wap_errno = -1;
	if (var->type != WAP_CONF_VAR_TYPE_FLOAT) {
		_wap_errno = WAP_ERR_INVALID_VAR_TYPE;
		return -1;
	}

	return var->content.value.floating;
}

bool wapi_var_as_boolean(_wap_t_config_variable *var) {
	_wap_errno = -1;
	if (var->type != WAP_CONF_VAR_TYPE_BOOLEAN) {
		_wap_errno = WAP_ERR_INVALID_VAR_TYPE;
		return false;
	}

	return var->content.value.boolean;
}