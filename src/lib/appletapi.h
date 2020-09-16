#ifndef WAPANEL_INCL_H_APPLETAPI
#define WAPANEL_INCL_H_APPLETAPI

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Variable types used in applet config struct
enum wap_t_config_var_type {
	WAP_CONF_VAR_TYPE_STRING = 0,
	WAP_CONF_VAR_TYPE_INTEGER = 1,
	WAP_CONF_VAR_TYPE_FLOAT = 2,
	WAP_CONF_VAR_TYPE_BOOLEAN = 3,
	WAP_CONF_VAR_TYPE_ARRAY = 4,
	WAP_CONF_VAR_TYPE_TABLE = 5,
};

// Entire toml-based applet config
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
			struct _wap_t_config_variable *_content = NULL;
		} array;
		struct table {
			const char *name;
			unsigned int _size;
			struct _wap_t_config_variable *_content = NULL;
		} table;
	} content;
} _wap_t_config_variable;

typedef struct wap_t_applet_config {
	unsigned int _size;
	_wap_t_config_variable *root = NULL;
} wap_t_applet_config;

// Basic informations about applet
typedef struct wap_t_applet_info {
	const char *name;
	unsigned int version;
} wap_t_applet_info;

// TODO: Add handy functions to manipulate applet config struct
#endif