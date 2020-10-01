#include <appletapi.h>
#include <gtk/gtk.h>
#include <vector>

/*
	Default config.

	[[panel.applet]]
		name = "separator"
		stretch = true
		margin_start = 0
		margin_end = 0
*/

extern "C" {

// Called to get handy info about this applet.
wap_t_applet_info wap_applet_info() { return { .name = "separator", .version = 1 }; }

// Called when some panel need new instance of your applet.
GtkWidget *wap_applet_new_instance(wap_t_applet_config applet_config) {
	GtkWidget *separator = gtk_label_new("");

	if (wapi_key_exists(&applet_config.root, "stretch")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "stretch");
		if (var->type == WAP_CONF_VAR_TYPE_BOOLEAN) { gtk_widget_set_hexpand(separator, wapi_var_as_boolean(var)); }
	} else {
		gtk_widget_set_hexpand(separator, true);
	}

	if (wapi_key_exists(&applet_config.root, "margin_start")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "margin_start");
		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) {
			gtk_widget_set_margin_start(separator, wapi_var_as_integer(var));
		}
	}

	if (wapi_key_exists(&applet_config.root, "margin_end")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "margin_end");
		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) { gtk_widget_set_margin_end(separator, wapi_var_as_integer(var)); }
	}

	return separator;
}

// Called when requested to remove all existing instances. GtkWidget should be disposed by panel.
void wap_event_remove_instances() {}

// Called when panel exits.
void wap_event_exit() {}
}