#include "activator.hh"
#include "../../log.hh"
#include "../double_fork.hh"
#include "appletapi.h"
#include <cstring>

#define use_wapi_string_variable(config, name, if_1, if_2)                                                             \
	if (wapi_key_exists(config, name)) {                                                                               \
		var = wapi_get_var_from_table(config, name);                                                                   \
                                                                                                                       \
		if (var->type != WAP_CONF_VAR_TYPE_STRING) {                                                                   \
			if_1                                                                                                       \
		} else {                                                                                                       \
			if_2                                                                                                       \
		}                                                                                                              \
	}

namespace wapanel::applet {

auto activator::create_activator_button(config::activator activator, bool is_listed) -> GtkButton * {
	GtkButton *activator_button = GTK_BUTTON(gtk_button_new());

	unsigned int icon_size = this->m_config.icon_height;

	if (is_listed) { icon_size /= 1.5; }

	GtkIconTheme *default_icon_theme = gtk_icon_theme_get_default();
	GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(default_icon_theme, activator.common_keys.icon.c_str(), icon_size,
												 GTK_ICON_LOOKUP_FORCE_REGULAR, NULL);

	GtkImage *image = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf));

	if (m_config.is_flat) { gtk_button_set_relief(activator_button, GTK_RELIEF_NONE); }

	if (is_listed) {
		GtkBox *container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));

		gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_START);

		gtk_box_pack_start(container, GTK_WIDGET(image), false, false, 0);
		gtk_box_pack_start(container, GTK_WIDGET(gtk_label_new(activator.common_keys.name.c_str())), false, false, 0);

		gtk_container_add(GTK_CONTAINER(activator_button), GTK_WIDGET(container));
	} else {
		gtk_container_add(GTK_CONTAINER(activator_button), GTK_WIDGET(image));
	}

	// Tooltip creation

	GValue val = G_VALUE_INIT;
	g_object_get_property(G_OBJECT(activator_button), "has-tooltip", &val);
	g_value_set_boolean(&val, true);
	g_object_set_property(G_OBJECT(activator_button), "has-tooltip", &val);

	std::string name_content = activator.common_keys.name;
	std::string description_content = activator.common_keys.description;

	std::string content = "<b>" + name_content + "</b>\n" + description_content;

	query_tooltip_data *data = new query_tooltip_data();
	data->icon_pixbuf = pixbuf;

	data->message_content = (char *)malloc(content.length() + 1);
	memcpy(data->message_content, content.c_str(), content.length() + 1);

	this->m_tooltip_data.push_back(data);

	g_signal_connect(activator_button, "query-tooltip",
					 G_CALLBACK(+[](GtkWidget *widget, int x, int y, gboolean keyboard_mode, GtkTooltip *tooltip,
									struct query_tooltip_data *data) {
						 gtk_tooltip_set_markup(tooltip, data->message_content);
						 gtk_tooltip_set_icon(tooltip, data->icon_pixbuf);

						 return true;
					 }),
					 data);

	if (activator.type == config::activator_type::APPLICATION) {
		std::string wd = std::get<0>(activator.application).working_directory;
		std::string cmd = std::get<0>(activator.application).command;

		clicked_data *data = new clicked_data();

		data->v1 = (char *)malloc(wd.length() + 1);
		data->v2 = (char *)malloc(cmd.length() + 1);

		memcpy(data->v1, wd.c_str(), wd.length() + 1);
		memcpy(data->v2, cmd.c_str(), cmd.length() + 1);

		m_clicked_data.push_back(data);

		g_signal_connect(activator_button, "clicked", G_CALLBACK(+[](GtkButton *button, struct clicked_data *data) {
							 if (utils::double_fork() == 0) {
								 chdir(data->v1);

								 system(data->v2);
								 exit(0);
							 }

							 return true;
						 }),
						 data);
	} else {
		std::string url = "xdg-open " + std::get<1>(activator.website).url;

		clicked_data *data = new clicked_data();

		data->v1 = (char *)malloc(url.length() + 1);
		memcpy(data->v1, url.c_str(), url.length() + 1);

		m_clicked_data.push_back(data);

		g_signal_connect(activator_button, "clicked", G_CALLBACK(+[](GtkButton *button, struct clicked_data *data) {
							 if (utils::double_fork() == 0) { system(data->v1); }

							 return true;
						 }),
						 data);
	}

	return activator_button;
}

auto activator::resolve_activator_configuration(_wap_t_config_variable *config) -> activator::config::activator {
	config::activator activator_struct_conf;
	_wap_t_config_variable *var = NULL;

	// Basic key used for initialization

	use_wapi_string_variable(
		config, "type",
		{
			activator_struct_conf.type = config::activator_type::NONE;
			return activator_struct_conf;
		},
		{
			std::string type_name = wapi_var_as_string(var);

			if (type_name == "application") {
				activator_struct_conf.type = config::activator_type::APPLICATION;
			} else if (type_name == "hypertext") {
				activator_struct_conf.type = config::activator_type::HYPERTEXT;
			} else {
				activator_struct_conf.type = config::activator_type::NONE;
				return activator_struct_conf;
			}
		}) else {
		activator_struct_conf.type = config::activator_type::NONE;
	}

	// Common keys used in all types

	use_wapi_string_variable(
		config, "name", { activator_struct_conf.common_keys.name = "Activator"; },
		{ activator_struct_conf.common_keys.name = wapi_var_as_string(var); });

	use_wapi_string_variable(
		config, "description",
		{ activator_struct_conf.common_keys.description = "Here should be your activator description"; },
		{ activator_struct_conf.common_keys.description = wapi_var_as_string(var); });

	use_wapi_string_variable(
		config, "icon", { activator_struct_conf.common_keys.icon = "application-x-executable"; },
		{ activator_struct_conf.common_keys.icon = wapi_var_as_string(var); });

	// Application specific keys

	config::app application;

	use_wapi_string_variable(
		config, "command", { application.command = ""; }, { application.command = wapi_var_as_string(var); });

	use_wapi_string_variable(
		config, "working_directory", { application.working_directory = ""; },
		{ application.working_directory = wapi_var_as_string(var); });

	// Hypertext specific keys

	config::hypertext hypertext;

	use_wapi_string_variable(
		config, "url", { hypertext.url = ""; }, { hypertext.url = wapi_var_as_string(var); });

	// Checking of type

	if (activator_struct_conf.type == config::activator_type::APPLICATION) {
		activator_struct_conf.application = application;
	} else {
		activator_struct_conf.website = hypertext;
	}

	return activator_struct_conf;
}

activator::activator(wap_t_applet_config &applet_config, int id) {
	_wap_t_config_variable *activator_array = NULL;

	log_info("Created activator instance");

	// Resolve config

	if (wapi_key_exists(&applet_config.root, "__panel_height")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "__panel_height");
		m_config.__panel_height = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "flat")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "flat");

		if (var->type == WAP_CONF_VAR_TYPE_BOOLEAN) m_config.is_flat = wapi_var_as_boolean(var);
	}

	if (wapi_key_exists(&applet_config.root, "icon_height")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "icon_height");
		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) m_config.icon_height = wapi_var_as_integer(var);
	}

	if (wapi_key_exists(&applet_config.root, "activator")) {
		activator_array = wapi_get_var_from_table(&applet_config.root, "activator");
	}

	if (activator_array != NULL && activator_array->type == WAP_CONF_VAR_TYPE_ARRAY) {
		unsigned int array_size = activator_array->content.array._size;
		_wap_t_config_variable *activator = NULL;

		for (size_t i = 0; i < array_size; i++) {

			activator = wapi_get_var_from_array(activator_array, i);
			if (activator == NULL || activator->type != WAP_CONF_VAR_TYPE_TABLE) { continue; }

			config::activator resolved_config = this->resolve_activator_configuration(activator);

			if (resolved_config.type == config::activator_type::NONE) { continue; }

			log_info("Configured activator `%s`", resolved_config.common_keys.name.c_str());

			m_config.activators.push_back(resolved_config);
		}
	}

	log_info("Resolved config");

	// Create gtk frontend

	this->m_container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

	unsigned int icon_size = m_config.__panel_height * 1.5;
	if (m_config.icon_height != -1) {
		icon_size = m_config.icon_height;
	} else {
		m_config.icon_height = icon_size;
	}

	if (m_config.activators.size() == 0) { return; }

	gtk_container_add(GTK_CONTAINER(m_container), GTK_WIDGET(create_activator_button(m_config.activators[0])));

	if (m_config.activators.size() == 1) { return; }

	m_menu_button = GTK_MENU_BUTTON(gtk_menu_button_new());
	m_menu_popover = GTK_POPOVER(gtk_popover_new(GTK_WIDGET(m_menu_button)));
	m_menu_popover_container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2));

	if (m_config.is_flat) { gtk_button_set_relief(GTK_BUTTON(m_menu_button), GTK_RELIEF_NONE); }

	for (size_t i = 1; i < m_config.activators.size(); i++) {
		GtkButton *button = create_activator_button(m_config.activators[i], true);
		gtk_box_pack_end(m_menu_popover_container, GTK_WIDGET(button), false, false, 0);

		log_info("Added hidden activator button");
	}

	gtk_container_add(GTK_CONTAINER(m_menu_popover), GTK_WIDGET(m_menu_popover_container));
	gtk_menu_button_set_popover(m_menu_button, GTK_WIDGET(m_menu_popover));
	gtk_widget_show_all(GTK_WIDGET(m_menu_popover_container));

	// Set GTK style things.
	GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_container));
	gtk_style_context_add_class(context, "activator");
	gtk_widget_set_name(GTK_WIDGET(m_container), std::string("activator-" + std::to_string(id)).c_str());

	// For popover
	context = gtk_widget_get_style_context(GTK_WIDGET(m_menu_popover));
	gtk_style_context_add_class(context, "activator-popover");
	gtk_widget_set_name(GTK_WIDGET(m_menu_popover), std::string("activator-popover-" + std::to_string(id)).c_str());

	gtk_container_add(GTK_CONTAINER(m_container), GTK_WIDGET(m_menu_button));
}

activator::~activator() {
	for (auto &&tooltip_data : this->m_tooltip_data) {
		free(tooltip_data->message_content);
		delete tooltip_data;

		log_info("Removed tooltip data");
	}

	for (auto &&clicked_data : this->m_clicked_data) {
		if (clicked_data->v1 != NULL) free(clicked_data->v1);
		if (clicked_data->v2 != NULL) free(clicked_data->v2);

		log_info("Removed clicked_data");

		delete clicked_data;
	}
}

auto activator::get_widget() -> GtkWidget * { return GTK_WIDGET(this->m_container); }

}