#pragma once
#include "backend.hh"
#include <appletapi.h>
#include <gtk/gtk.h>

namespace wapanel::applet {

class volume_control {
private:
	backend *m_backend;
	GtkToggleButton *m_toggle_btn;
	GtkImage *m_button_icon;

	struct config {
		unsigned int __panel_height;

		int icon_height;
		bool is_flat;
	} m_config;

public:
	volume_control(wap_t_applet_config applet_config, backend *backend);
	~volume_control();

	auto get_widget() -> GtkWidget *;
};

} // namespace wapanel::applet::volume_control
