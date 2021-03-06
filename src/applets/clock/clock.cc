#include "clock.hh"
#include "../../log.hh"
#include <string>

namespace wapanel::applet {

auto resolve_font_config(wap_t_applet_config applet_config) -> PangoAttrList * {
	auto attr_list = pango_attr_list_new();
	_wap_t_config_variable *font_table;
	_wap_t_config_variable *var;

	if (!wapi_key_exists(&applet_config.root, "font")) {
		pango_attr_list_insert(attr_list, pango_attr_family_new(""));
		pango_attr_list_insert(attr_list, pango_attr_style_new(PANGO_STYLE_NORMAL));
		pango_attr_list_insert(attr_list, pango_attr_weight_new(PANGO_WEIGHT_BOLD));

		return attr_list;
	}

	font_table = wapi_get_var_from_table(&applet_config.root, "font");

	if (wapi_key_exists(font_table, "family")) {
		var = wapi_get_var_from_table(font_table, "family");
		const char *family;

		if (var->type == WAP_CONF_VAR_TYPE_STRING) {
			family = wapi_var_as_string(var);

			pango_attr_list_insert(attr_list, pango_attr_family_new(family));
		}
	} else {
		pango_attr_list_insert(attr_list, pango_attr_family_new(""));
	}

	if (wapi_key_exists(font_table, "style")) {
		var = wapi_get_var_from_table(font_table, "style");
		const char *style;

		if (var->type == WAP_CONF_VAR_TYPE_STRING) {
			style = wapi_var_as_string(var);

			if (!strcmp(style, "normal")) {
				pango_attr_list_insert(attr_list, pango_attr_style_new(PANGO_STYLE_NORMAL));
			} else if (!strcmp(style, "oblique")) {
				pango_attr_list_insert(attr_list, pango_attr_style_new(PANGO_STYLE_OBLIQUE));
			} else if (!strcmp(style, "italic")) {
				pango_attr_list_insert(attr_list, pango_attr_style_new(PANGO_STYLE_ITALIC));
			}
		}
	}

	if (wapi_key_exists(font_table, "weight")) {
		var = wapi_get_var_from_table(font_table, "weight");
		const char *weight;
		PangoAttribute *attribute;

		if (var->type == WAP_CONF_VAR_TYPE_STRING) {
			weight = wapi_var_as_string(var);

			if (!strcmp(weight, "thin")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_THIN);
			} else if (!strcmp(weight, "ultralight")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_ULTRALIGHT);
			} else if (!strcmp(weight, "light")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_LIGHT);
			} else if (!strcmp(weight, "semilight")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_SEMILIGHT);
			} else if (!strcmp(weight, "book")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_BOOK);
			} else if (!strcmp(weight, "normal")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_NORMAL);
			} else if (!strcmp(weight, "medium")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_MEDIUM);
			} else if (!strcmp(weight, "semibold")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_SEMIBOLD);
			} else if (!strcmp(weight, "bold")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
			} else if (!strcmp(weight, "ultrabold")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_ULTRABOLD);
			} else if (!strcmp(weight, "heavy")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_HEAVY);
			} else if (!strcmp(weight, "ultraheavy")) {
				attribute = pango_attr_weight_new(PANGO_WEIGHT_ULTRAHEAVY);
			} else {
				// Default attrib
				attribute = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
			}

			pango_attr_list_insert(attr_list, attribute);
		}
	}

	if (wapi_key_exists(font_table, "size")) {
		var = wapi_get_var_from_table(font_table, "size");
		int64_t size;

		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) {
			size = wapi_var_as_integer(var);

			if (size > -1) { pango_attr_list_insert(attr_list, pango_attr_size_new_absolute(size * PANGO_SCALE)); }
		}
	}

	if (wapi_key_exists(font_table, "strikethrough")) {
		var = wapi_get_var_from_table(font_table, "strikethrough");
		bool strikethrough;

		if (var->type == WAP_CONF_VAR_TYPE_BOOLEAN) {
			strikethrough = wapi_var_as_boolean(var);

			pango_attr_list_insert(attr_list, pango_attr_strikethrough_new(strikethrough));
		}
	}

	if (wapi_key_exists(font_table, "underline")) {
		var = wapi_get_var_from_table(font_table, "underline");
		const char *underline;
		PangoAttribute *attribute;

		if (var->type == WAP_CONF_VAR_TYPE_STRING) {
			underline = wapi_var_as_string(var);

			if (!strcmp(underline, "none")) {
				attribute = pango_attr_underline_new(PANGO_UNDERLINE_NONE);
			} else if (!strcmp(underline, "single")) {
				attribute = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
			} else if (!strcmp(underline, "double")) {
				attribute = pango_attr_underline_new(PANGO_UNDERLINE_DOUBLE);
			} else if (!strcmp(underline, "low")) {
				attribute = pango_attr_underline_new(PANGO_UNDERLINE_LOW);
			} else if (!strcmp(underline, "error")) {
				attribute = pango_attr_underline_new(PANGO_UNDERLINE_ERROR);
			} else {
				attribute = pango_attr_underline_new(PANGO_UNDERLINE_NONE);
			}

			pango_attr_list_insert(attr_list, attribute);
		}
	}

	if (wapi_key_exists(font_table, "letter_spacing")) {
		var = wapi_get_var_from_table(font_table, "letter_spacing");
		int64_t letter_spacing;

		if (var->type == WAP_CONF_VAR_TYPE_INTEGER) {
			letter_spacing = wapi_var_as_integer(var);

			if (letter_spacing > -1) {
				pango_attr_list_insert(attr_list, pango_attr_letter_spacing_new(letter_spacing * PANGO_SCALE));
			}
		}
	}

	return attr_list;
}

clock::clock(wap_t_applet_config applet_config, int id) {
	// Create instances of required widgets.
	m_clock_button = GTK_BUTTON(gtk_button_new());
	m_clock_label = GTK_LABEL(gtk_label_new(""));
	m_gtk_timeout = g_timeout_add(1000, G_SOURCE_FUNC(m_timeout_callback), this);

	log_info("Created gtk_timeout and necessary variables");
	log_info("Checking for existing time formatting in config");

	// Configure time formatting
	if (wapi_key_exists(&applet_config.root, "format")) {
		_wap_t_config_variable *var = wapi_get_var_from_table(&applet_config.root, "format");
		const char *format;

		if (var->type == WAP_CONF_VAR_TYPE_STRING) {
			format = wapi_var_as_string(var);

			if (!strcmp(format, "")) { format = "%X"; }

			m_time_format = reinterpret_cast<char *>(malloc(sizeof(format) + 16));
			strcpy(m_time_format, format);
		}
	} else {
		m_time_format = reinterpret_cast<char *>(malloc(sizeof("%X") + 1));
		strcpy(m_time_format, "%X");
	}

	log_info("First clock update call");

	// Pre-call to callback making label "visible".
	on_timeout();

	// Configure toggle button.
	gtk_widget_set_can_default(GTK_WIDGET(m_clock_button), FALSE);
	gtk_widget_set_can_focus(GTK_WIDGET(m_clock_button), FALSE);
	gtk_button_set_relief(m_clock_button, GTK_RELIEF_NONE);
	gtk_widget_set_focus_on_click(GTK_WIDGET(m_clock_button), FALSE);

	g_signal_connect(m_clock_button, "clicked", G_CALLBACK(m_clock_button_clicked), this);

	// Configure popover calendar
	m_calendar_popover = GTK_POPOVER(gtk_popover_new(GTK_WIDGET(m_clock_button)));

	m_calendar = GTK_CALENDAR(gtk_calendar_new());

	gtk_widget_set_size_request(GTK_WIDGET(m_calendar), 250, -1);
	gtk_calendar_set_display_options(m_calendar,
									 (GtkCalendarDisplayOptions)(GTK_CALENDAR_SHOW_HEADING | GTK_CALENDAR_SHOW_DAY_NAMES
																 | GTK_CALENDAR_SHOW_WEEK_NUMBERS));

	gtk_container_add(GTK_CONTAINER(m_calendar_popover), GTK_WIDGET(m_calendar));

	log_info("Spawned and added calendar");

	// Configure and add label to button.
	m_label_attr = resolve_font_config(applet_config);
	gtk_label_set_attributes(m_clock_label, m_label_attr);
	gtk_container_add(GTK_CONTAINER(m_clock_button), GTK_WIDGET(m_clock_label));

	// GTK style things
	GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_clock_button));
	gtk_style_context_add_class(context, "clock");
	gtk_widget_set_name(GTK_WIDGET(m_clock_button), std::string("clock-" + std::to_string(id)).c_str());

	// For popover
	context = gtk_widget_get_style_context(GTK_WIDGET(m_calendar_popover));
	gtk_style_context_add_class(context, "clock-popover");
	gtk_widget_set_name(GTK_WIDGET(m_calendar_popover), std::string("clock-popover-" + std::to_string(id)).c_str());
}

clock::~clock() {
	g_source_remove(m_gtk_timeout);
	gtk_widget_destroy(GTK_WIDGET(m_clock_button));

	pango_attr_list_unref(m_label_attr);
	free(m_time_format);
}

auto clock::get_widget() -> GtkWidget * { return GTK_WIDGET(m_clock_button); }

auto clock::on_timeout() -> void {
	m_unixtime = time(NULL);
	if (localtime_r(&m_unixtime, &m_time_conv) == NULL) {
		log_error("Error in localtime_r");
		return;
	}

	strftime(m_str_time, sizeof(m_str_time), m_time_format, &m_time_conv);
	gtk_label_set_text(m_clock_label, m_str_time);
}

auto clock::on_clicked() -> void {
	m_unixtime = time(NULL);
	if (localtime_r(&m_unixtime, &m_time_conv) == NULL) {
		log_error("Error in localtime_r");
		return;
	}

	gtk_calendar_select_day(m_calendar, m_time_conv.tm_mday);
	gtk_calendar_select_month(m_calendar, m_time_conv.tm_mon, m_time_conv.tm_year + 1900);

	gtk_popover_popup(m_calendar_popover);
	gtk_widget_show_all(GTK_WIDGET(m_calendar_popover));
}

}