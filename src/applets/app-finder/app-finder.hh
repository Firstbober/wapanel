#pragma once
#include <appletapi.h>
#include <gtk/gtk.h>
#include <string>

#include "components/action_bar.hh"
#include "components/list_area.hh"
#include "components/logout_box.hh"

/*
	Default config

	[[panel.applet]]
		name = "app-finder"

		icon = ""
		icon_height = -1
		flat = false

		popover_width = -1
		popover_height = -1

		user_manager = ""
		settings = ""
		file_manager = ""

		[panel.applet.logout]
			shutdown = ""
			restart = ""
			logout = ""
			suspend = ""
			hibernate = ""
			lock = ""
*/

namespace wapanel::applet {

class app_finder {
private:
	int m_id;

	GtkMenuButton *m_app_finder_btn;
	GtkImage *m_btn_icon;
	GtkPopover *m_finder_popover;
	GtkBox *m_finder_aligner;

	ui_comps::action_bar *m_sidebar;
	ui_comps::list_area *m_list_area;

	struct config {
		int __panel_height;

		std::string icon_name;
		int icon_height = -1;
		bool is_flat;

		int popover_width = -1;
		int popover_height = -1;
	} m_config;

public:
	app_finder(wap_t_applet_config applet_config, int id);
	~app_finder();

	auto get_widget() -> GtkWidget *;
};

} // namespace wapanel::applet
