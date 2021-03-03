#pragma once
#include <gtk/gtk.h>

namespace ui_comps {

class list_area {
private:
	GtkBox *m_root;
	GtkNotebook *m_list_container;
	GtkStack *m_view_stack;
	GtkSearchEntry *m_search_entry;

public:
	list_area(int apid);
	~list_area();

	auto get_widget() -> GtkWidget *;
};

};