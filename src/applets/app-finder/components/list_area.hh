#pragma once
#include "../app-entry.hh"
#include "../search-engine/search_engine.hh"
#include <cstddef>
#include <gtk/gtk.h>
#include <map>
#include <vector>

namespace ui_comps {

class application_list {

private:
	GtkScrolledWindow *m_scroll_win;
	GtkViewport *m_viewport;
	GtkListBox *m_app_list;

	std::vector<AppEntry> m_entries;

public:
	application_list(int apid, GtkPopover *tl_popover);

	auto get_widget() -> GtkWidget *;
	auto add_app(AppEntry entry);
};

class list_area {
private:
	GtkBox *m_root;
	GtkNotebook *m_list_container;
	GtkStack *m_view_stack;
	GtkSearchEntry *m_search_entry;

	se::SearchEngine m_se;
	bool m_found_all_entries = false;

	std::map<Category, application_list> m_app_lists;
	std::map<Category, int> m_notebook_pages;

public:
	list_area(int apid, GtkPopover *tl_popover);
	~list_area();

	auto get_widget() -> GtkWidget *;
};

};