#include "utils.hh"
#include <gtk/gtk.h>

namespace wapanel {

auto spawn_gtk_error(std::string message) -> void {
	auto dialog_window = gtk_window_new(GTK_WINDOW_POPUP);
	auto dialog = gtk_message_dialog_new(GTK_WINDOW(dialog_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
										 GTK_BUTTONS_OK, message.c_str());

	int result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog_window);

	if (result == GTK_RESPONSE_OK) { return; }
}

}