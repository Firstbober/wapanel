#include "list_area.hh"

namespace ui_comps {
list_area::list_area()
	: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 4)))
	, m_list_container(GTK_NOTEBOOK(gtk_notebook_new())) {}
list_area::~list_area() {}
auto list_area::get_widget() -> GtkWidget * { return GTK_WIDGET(m_list_container); }
};