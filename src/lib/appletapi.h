#ifndef WAPANEL_INCL_H_APPLETAPI
#define WAPANEL_INCL_H_APPLETAPI

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Variable types used in applet config struct.
typedef enum wap_t_config_var_type {
	WAP_CONF_VAR_TYPE_STRING = 0,
	WAP_CONF_VAR_TYPE_INTEGER = 1,
	WAP_CONF_VAR_TYPE_FLOAT = 2,
	WAP_CONF_VAR_TYPE_BOOLEAN = 3,
	WAP_CONF_VAR_TYPE_ARRAY = 4,
	WAP_CONF_VAR_TYPE_TABLE = 5
} wap_t_config_var_type;

// Entire toml-based applet config.
typedef struct _wap_t_config_variable {
	union key {
		const char *string;
		unsigned int number;
	} key;
	wap_t_config_var_type type;

	union content {
		union value {
			const char *string;
			int64_t integer;
			double floating;
			bool boolean;
		} value;
		struct array {
			unsigned int _size;
			struct _wap_t_config_variable *_content;
		} array;
		struct table {
			unsigned int _size;
			struct _wap_t_config_variable *_content;
		} table;
	} content;
} _wap_t_config_variable;

typedef struct wap_t_applet_config {
	_wap_t_config_variable root;
} wap_t_applet_config;

// Basic informations about applet.
typedef struct wap_t_applet_info {
	const char *name;
	unsigned int version;
} wap_t_applet_info;

// Enum for errors.
typedef enum wap_t_error_type {
	WAP_ERR_VARIABLE_IS_NOT_TABLE = 0,
	WAP_ERR_VARIABLE_IS_NOT_ARRAY = 1,
	WAP_ERR_CANNOT_FIND = 2,
	WAP_ERR_INVALID_VAR_TYPE = 3
} wap_t_error_type;

extern wap_t_error_type _wap_errno;

#ifdef __cplusplus
extern "C" {
#endif

// Check if errno is set to error,
// returns true if error is present.
bool wapi_error();

// Prints error if it's present.
const char *wapi_error_print();

// Checks if specified key exists in passed table.
bool wapi_key_exists(_wap_t_config_variable *table, const char *key);
// Checks if specified index exists inside passed array.
bool wapi_index_exists(_wap_t_config_variable *array, size_t index);

// Extracts variable with specified key from table.
_wap_t_config_variable *wapi_get_var_from_table(_wap_t_config_variable *table, const char *key);
// Extracts variable with specified index from array.
_wap_t_config_variable *wapi_get_var_from_array(_wap_t_config_variable *array, size_t index);

const char *wapi_var_as_string(_wap_t_config_variable *var);
int64_t wapi_var_as_integer(_wap_t_config_variable *var);
double wapi_var_as_floating(_wap_t_config_variable *var);
bool wapi_var_as_boolean(_wap_t_config_variable *var);

#ifdef __cplusplus
}
#endif

#endif