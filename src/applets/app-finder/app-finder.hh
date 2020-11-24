#pragma once
#include <appletapi.h>
#include <gtk/gtk.h>

namespace wapanel::applet {

class app_finder {
private:
	int m_id;

public:
	app_finder(wap_t_applet_config applet_config, int id);
	~app_finder();

	auto get_widget() -> GtkWidget*;
};

} // namespace wapanel::applet
