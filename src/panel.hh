#pragma once

#include "config.hh"
#include <gtk/gtk.h>

namespace wapanel {
class panel {
private:
	GtkWindow *m_window;
	GtkBox *m_appletbox;

	unsigned int m_id;

public:
	// !!!! DO NOT USE GLOBAL VALUES FOR YOUR OWN SANITY !!!!
	panel(unsigned int id, conf::global_config *config);
	// !!!! END OF TRANSMISSION !!!!

	~panel();

	auto get_gtk_window() -> GtkWindow *;

	// !!!! DO NOT USE GLOBAL VALUES FOR YOUR OWN SANITY !!!!
	auto configure(conf::global_config *config) -> void;
	// !!!! END OF TRANSMISSION !!!!
};
}