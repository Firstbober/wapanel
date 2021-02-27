#pragma once
#include <gtk/gtk.h>

namespace ui_comps {

class list_area {
private:
	GtkBox *m_root;
	GtkNotebook *m_list_container;

public:
	list_area();
	~list_area();

	auto get_widget() -> GtkWidget *;
};

};