#include "list_area.hh"
#include "../../icon_cache.hh"
#include "../app-entry.hh"
#include <string>

auto construct_category_label(std::string icon_name, std::string category_name) -> GtkWidget * {
	GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));

	gtk_box_pack_start(box, gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon(icon_name, 20)), false, true,
					   0);
	gtk_box_pack_start(box, gtk_label_new(category_name.c_str()), false, false, 0);

	gtk_widget_show_all(GTK_WIDGET(box));

	return GTK_WIDGET(box);
}

struct application_list {
	GtkScrolledWindow *scroll_win;
	GtkViewport *viewport;
	GtkListBox *app_list;

	application_list()
		: scroll_win(GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL)))
		, viewport(GTK_VIEWPORT(gtk_viewport_new(NULL, NULL)))
		, app_list(GTK_LIST_BOX(gtk_list_box_new())) {

		gtk_container_add(GTK_CONTAINER(scroll_win), GTK_WIDGET(viewport));
		gtk_container_add(GTK_CONTAINER(viewport), GTK_WIDGET(app_list));

		gtk_widget_show_all(GTK_WIDGET(scroll_win));
	}

	auto get_widget() -> GtkWidget * { return GTK_WIDGET(scroll_win); }

	auto add_app(AppEntry entry) {}
};

auto construct_app_list() -> application_list {}

namespace ui_comps {
list_area::list_area(int apid)
	: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6)))
	, m_list_container(GTK_NOTEBOOK(gtk_notebook_new()))
	, m_view_stack(GTK_STACK(gtk_stack_new()))
	, m_search_entry(GTK_SEARCH_ENTRY(gtk_search_entry_new())) {

	// Initialize everything

	gtk_notebook_set_tab_pos(this->m_list_container, GTK_POS_RIGHT);
	gtk_stack_add_named(this->m_view_stack, GTK_WIDGET(this->m_list_container), "list-with-category");
	gtk_stack_set_visible_child_name(this->m_view_stack, "list-with-category");

	gtk_entry_set_placeholder_text(GTK_ENTRY(this->m_search_entry), "Search...");

	gtk_box_pack_end(this->m_root, GTK_WIDGET(this->m_search_entry), false, false, 0);
	gtk_box_pack_end(this->m_root, GTK_WIDGET(this->m_view_stack), true, true, 0);

	gtk_widget_show_all(GTK_WIDGET(this->m_root));

	// Some dummy notebook tests

	gtk_notebook_append_page(this->m_list_container, gtk_label_new("AAAAAA"),
							 construct_category_label("applications-utilities-symbolic", "Accessories"));
	gtk_notebook_append_page(this->m_list_container, gtk_label_new("AAAAAA"),
							 construct_category_label("applications-graphics-symbolic", "Graphics"));
	gtk_notebook_append_page(this->m_list_container, gtk_label_new("AAAAAA"),
							 construct_category_label("applications-system-symbolic", "System"));

	GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_list_container));
	gtk_style_context_add_class(context, "app-finder-category-notebook");
	gtk_widget_set_name(GTK_WIDGET(m_list_container),
						std::string("app-finder-category-notebook-" + std::to_string(apid)).c_str());
}
list_area::~list_area() {}
auto list_area::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }
};