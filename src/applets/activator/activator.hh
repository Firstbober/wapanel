#pragma once
#include <appletapi.h>
#include <gtk/gtk.h>
#include <string>
#include <variant>
#include <vector>

/*
	Default config
	-1 stands for default value

	[[panel.applet]]
		name = "activator"
		flat = false
		icon_height = -1

		[[panel.applet.activator]]
			type = "application"

			name = "Screenshooter"
			description = "Put something here"
			icon = "application-x-executable"

			command = "xfce4-screenshooter"
			working_directory = ""
*/

namespace wapanel::applet {

class activator {
private:
	GtkBox *m_container;
	GtkMenuButton *m_menu_button;
	GtkBox *m_menu_popover_container;
	GtkPopover *m_menu_popover;

	struct config {
		struct common {
			std::string name;
			std::string description;
			std::string icon;
		};

		struct app {
			std::string command;
			std::string working_directory;
		};
		struct hypertext {
			std::string url;
		};

		enum class activator_type { APPLICATION = 0, HYPERTEXT = 1, NONE = 2 };

		struct activator {
			activator_type type;
			common common_keys;

			std::variant<app, hypertext> application, website;
		};

		bool is_flat = false;

		unsigned int __panel_height;
		int icon_height;

		std::vector<activator> activators;
	} m_config;

	struct query_tooltip_data {
		GdkPixbuf *icon_pixbuf;
		char *message_content;
	};

	struct clicked_data {
		char *v1 = NULL;
		char *v2 = NULL;
	};

	std::vector<query_tooltip_data *> m_tooltip_data;
	std::vector<clicked_data *> m_clicked_data;

	auto resolve_activator_configuration(_wap_t_config_variable *config) -> config::activator;
	auto create_activator_button(config::activator activator, bool is_listed = false) -> GtkButton *;

public:
	activator(wap_t_applet_config &applet_config, int id);
	~activator();

	auto get_widget() -> GtkWidget *;
};

}