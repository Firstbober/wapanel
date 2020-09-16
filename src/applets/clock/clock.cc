#include <gtk/gtk.h>
#include <stdio.h>

#include <appletapi.h>

extern "C" {

// Called to get handy info about this applet
wap_t_applet_info wap_applet_info() { return { .name = "clock", .version = 1 }; }

// Called when some panel need new instance of your applet
GtkWidget *wap_applet_new_instance(wap_t_applet_config *applet_config) {
	printf("%s\n", applet_config->root[0].content.value.string);

	return gtk_label_new("jebac pis 123  ");
}

// Called when requested to remove all existing instances. GtkWidget should be disposed by panel
void wap_event_remove_instances() {}

// Called when panel exits
void wap_event_exit() {}
}