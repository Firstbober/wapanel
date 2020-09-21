#pragma once
#include <appletapi.h>
#include <gtk/gtk.h>

/*
	Default config.
	-1 stands for default value.

	For more docs go to
	https://developer.gnome.org/pango/stable/pango-Text-Attributes.html

[[panel.applet]]
	name = "clock"
	format = ""
	[panel.applet.font]
		family = ""
		style = ""
		weight = ""
		size = -1
		strikethrough = false
		underline = ""
		letter_spacing = -1
*/

namespace wapanel::applet {

class clock {
private:
	gint m_gtk_timeout;
	PangoAttrList *m_label_attr;

	GtkButton *m_clock_button;
	GtkLabel *m_clock_label;
	GtkPopover *m_calendar_popover;
	GtkCalendar *m_calendar;

	char m_str_time[256];
	char *m_time_format;
	time_t m_unixtime;
	struct tm *m_time_conv;

public:
	clock(wap_t_applet_config applet_config);
	~clock();

	// Returns widget used by panel.
	auto get_widget() -> GtkWidget *;

	// Real timeout callback.
	auto on_timeout() -> void;

	// Real clicked callback
	auto on_clicked() -> void;

private:
	auto static m_timeout_callback(clock *cl) -> void { cl->on_timeout(); }
	auto static m_clock_button_clicked(GtkButton *btn, clock *cl) -> void { cl->on_clicked(); }
};

}
