#pragma once

#include "config.hh"
#include "lib/appletapi.h"

#include <gtk/gtk.h>

namespace wapanel {
class panel {
private:
	GtkWindow *m_window;
	GtkBox *m_appletbox;

	unsigned int m_id;
	std::vector<_wap_t_config_variable *> m_last_applet_config_variables;

public:
	panel(unsigned int id);
	~panel();

	auto get_gtk_window() -> GtkWindow *;
	auto configure() -> void;
};
}