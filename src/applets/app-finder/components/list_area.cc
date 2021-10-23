#include "list_area.hh"
#include "../../icon_cache.hh"
#include "app-entry.hh"
#include "search-engine/search_engine.hh"
#include "search-engine/search_engine_mime.hh"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <gio/gdesktopappinfo.h>
#include <memory>
#include <string>
#include <unistd.h>
#include <utility>

std::string &ltrim(std::string &s) {
	auto it = std::find_if(s.begin(), s.end(), [](char c) { return !std::isspace<char>(c, std::locale::classic()); });
	s.erase(s.begin(), it);
	return s;
}

auto construct_category_label(std::string icon_name, std::string category_name) -> GtkWidget * {
	GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));

	gtk_box_pack_start(box, gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon(icon_name, 20)), false, true,
					   0);
	gtk_box_pack_start(box, gtk_label_new(category_name.c_str()), false, false, 0);

	gtk_widget_show_all(GTK_WIDGET(box));

	return GTK_WIDGET(box);
}

namespace ui_comps {

// Application list

struct app_list_row_label_data {
	std::string title;
	std::string desc;
	bool found_first;
	bool found_second;
};

auto get_label_content_from_app_list_row(GtkListBoxRow *row, app_list_row_label_data *data) {
	data->found_first = false;
	data->found_second = false;

	gtk_container_foreach(
		GTK_CONTAINER(gtk_bin_get_child(GTK_BIN(row))),
		+[](GtkWidget *w, void *data) {
			if (GTK_IS_BOX(w)) {
				gtk_container_foreach(
					GTK_CONTAINER(w),
					+[](GtkWidget *w, void *data) {
						auto dn = (app_list_row_label_data *)data;

						if (GTK_IS_LABEL(w)) {
							if (dn->found_first == false) {
								dn->title = gtk_label_get_text(GTK_LABEL(w));
								dn->found_first = true;
							} else {
								dn->found_second = true;
								dn->desc = gtk_label_get_text(GTK_LABEL(w));
							}
						}
					},
					data);
			}
		},
		data);
}

application_list::application_list(int apid, GtkPopover *tl_popover)
	: m_scroll_win(GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL)))
	, m_viewport(GTK_VIEWPORT(gtk_viewport_new(NULL, NULL)))
	, m_app_list(GTK_LIST_BOX(gtk_list_box_new())) {
	gtk_list_box_set_selection_mode(m_app_list, GTK_SELECTION_SINGLE);
	gtk_list_box_set_activate_on_single_click(m_app_list, true);

	gtk_container_add(GTK_CONTAINER(m_scroll_win), GTK_WIDGET(m_viewport));
	gtk_container_add(GTK_CONTAINER(m_viewport), GTK_WIDGET(m_app_list));

	// gtk_widget_show_all(GTK_WIDGET(m_scroll_win));

	// Connect row-activated signal

	g_signal_connect(
		m_app_list, "row-activated", G_CALLBACK(+[](GtkListBox *box, GtkListBoxRow *row, GtkPopover *tl_popover) {
			GObject *box_obj = G_OBJECT(gtk_bin_get_child(GTK_BIN(row)));

			char *_exec = (char *)g_object_get_data(box_obj, "_exec");
			char *_working_path = (char *)g_object_get_data(box_obj, "_working_path");

			g_spawn_command_line_async(
				std::string("sh -c \"cd " + std::string(_working_path) + ";" + std::string(_exec) + "\"").c_str(),
				NULL);

			gtk_popover_popdown(tl_popover);
		}),
		tl_popover);

	gtk_list_box_set_sort_func(
		m_app_list, GtkListBoxSortFunc([](GtkListBoxRow *r1, GtkListBoxRow *r2, void *data) -> int {
			auto row1_data = new app_list_row_label_data;
			get_label_content_from_app_list_row(r1, row1_data);

			auto row2_data = new app_list_row_label_data;
			get_label_content_from_app_list_row(r2, row2_data);

			auto ret_val = 0;

			std::transform(row1_data->title.begin(), row1_data->title.end(), row1_data->title.begin(), ::tolower);
			std::transform(row2_data->title.begin(), row2_data->title.end(), row2_data->title.begin(), ::tolower);

			if (row1_data->found_first && row2_data->found_first) {
				if (row1_data->title > row2_data->title) {
					ret_val = 1;
				} else {
					ret_val = -1;
				}
			}

			delete row1_data;
			delete row2_data;

			return ret_val;
		}),
		NULL, NULL);

	// Styling

	GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_scroll_win));
	gtk_style_context_add_class(context, "app-finder-category-app-list");
	gtk_widget_set_name(GTK_WIDGET(m_scroll_win),
						std::string("app-finder-category-app-list-" + std::to_string(apid)).c_str());
}

auto application_list::get_widget() -> GtkWidget * { return GTK_WIDGET(m_scroll_win); }
auto application_list::add_app(AppEntry entry) {
	for (auto &&_entry : this->m_entries) {
		if (entry.name == _entry.name && entry.exec == _entry.exec) { return; }
	}

	if (entry.working_path.empty()) { entry.working_path = "."; }

	GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));

	gtk_box_pack_start(box, gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon(entry.iconstring, 28)),
					   false, true, 0);

	GtkBox *title_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

	GtkLabel *title = GTK_LABEL(gtk_label_new(entry.name.c_str()));
	gtk_label_set_ellipsize(title, PANGO_ELLIPSIZE_END);

	GtkLabel *desc = GTK_LABEL(gtk_label_new(ltrim(entry.description).c_str()));
	gtk_label_set_ellipsize(desc, PANGO_ELLIPSIZE_END);

	gtk_widget_set_halign(GTK_WIDGET(title), GTK_ALIGN_START);
	gtk_widget_set_halign(GTK_WIDGET(desc), GTK_ALIGN_START);

	gtk_box_pack_start(title_box, GTK_WIDGET(title), false, false, 0);
	if (!entry.description.empty()) gtk_box_pack_start(title_box, GTK_WIDGET(desc), false, false, 0);

	gtk_box_pack_start(box, GTK_WIDGET(title_box), false, false, 0);

	gtk_widget_set_margin_top(GTK_WIDGET(box), 2);
	gtk_widget_set_margin_bottom(GTK_WIDGET(box), 2);
	gtk_widget_set_margin_start(GTK_WIDGET(box), 4);
	gtk_widget_set_margin_end(GTK_WIDGET(box), 4);

	gtk_widget_show_all(GTK_WIDGET(box));

	char *cpy = (char *)malloc(entry.exec.length() + 1);
	strncpy(cpy, entry.exec.c_str(), entry.exec.length() + 1);

	g_object_set_data(G_OBJECT(box), "_exec", cpy);

	cpy = (char *)malloc(entry.working_path.length() + 1);
	strncpy(cpy, entry.working_path.c_str(), entry.working_path.length() + 1);

	g_object_set_data(G_OBJECT(box), "_working_path", cpy);

	gtk_widget_set_tooltip_text(GTK_WIDGET(box), entry.name.c_str());
	gtk_list_box_insert(m_app_list, GTK_WIDGET(box), -1);

	this->m_entries.push_back(entry);
}

inline auto charp_to_string(char *str) -> std::string {
	if (str == NULL) {
		return {};
	} else {
		return str;
	}
}

auto split(std::string text, const char *separator) -> std::vector<std::string> {
	std::vector<std::string> splitted;

	char *cpy = (char *)malloc(text.length() + 1);
	strncpy(cpy, text.c_str(), text.length() + 1);

	char *token = strtok(cpy, separator);

	while (token != NULL) {
		splitted.push_back(token);
		token = strtok(NULL, separator);
	}

	free(cpy);

	return splitted;
}

std::array categories_tps { Category::Accessories, Category::Development, Category::Education, Category::Game,
							Category::Graphics,	   Category::Multimedia,  Category::Network,   Category::Office,
							Category::Science,	   Category::Settings,	  Category::System,	   Category::Other };

std::array category_icon { "applications-utilities-symbolic", "applications-engineering-symbolic",
						   "x-office-spreadsheet-symbolic",	  "applications-games-symbolic",
						   "applications-graphics-symbolic",  "applications-multimedia-symbolic",
						   "network-workgroup-symbolic",	  "x-office-presentation-symbolic",
						   "applications-science-symbolic",	  "applications-system-symbolic",
						   "preferences-system-symbolic",	  "preferences-other-symbolic" };

std::array category_name { "Accessories", "Development", "Education", "Games",	  "Graphics", "Multimedia",
						   "Network",	  "Office",		 "Science",	  "Settings", "System",	  "Other" };

bool replace(std::string &str, const std::string &from, const std::string &to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos) return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

// https://stackoverflow.com/questions/3152241/case-insensitive-stdstring-find

template <typename charT> struct my_equal {
	my_equal(const std::locale &loc)
		: loc_(loc) {}
	bool operator()(charT ch1, charT ch2) { return std::toupper(ch1, loc_) == std::toupper(ch2, loc_); }

private:
	const std::locale &loc_;
};

template <typename T> int ci_find_substr(const T &str1, const T &str2, const std::locale &loc = std::locale()) {
	typename T::const_iterator it
		= std::search(str1.begin(), str1.end(), str2.begin(), str2.end(), my_equal<typename T::value_type>(loc));
	if (it != str1.end())
		return it - str1.begin();
	else
		return -1; // not found
}

// https://stackoverflow.com/questions/3152241/case-insensitive-stdstring-find

list_area::list_area(int apid, GtkPopover *tl_popover)
	: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6)))
	, m_list_container(GTK_NOTEBOOK(gtk_notebook_new()))
	, m_view_stack(GTK_STACK(gtk_stack_new()))
	, m_search_entry(GTK_SEARCH_ENTRY(gtk_search_entry_new()))
	, m_search_list(new application_list(apid, tl_popover))
	, m_se(new se::SearchEngine()) {
	(*this->m_search_container_visible) = false;

	// Initialize category notebook

	gtk_notebook_set_tab_pos(this->m_list_container, GTK_POS_RIGHT);
	gtk_stack_add_named(this->m_view_stack, GTK_WIDGET(this->m_list_container), "list-with-category");
	gtk_stack_add_named(this->m_view_stack, GTK_WIDGET(this->m_search_list->get_widget()), "search-list");

	gtk_stack_set_visible_child_name(this->m_view_stack, "list-with-category");

	// Pack everything to m_root and show all

	gtk_box_pack_end(this->m_root, GTK_WIDGET(this->m_search_entry), false, true, 0);
	gtk_box_pack_end(this->m_root, GTK_WIDGET(this->m_view_stack), true, true, 0);

	gtk_widget_show_all(GTK_WIDGET(this->m_root));

	// Register event for search

	auto sc_d = new search_changed_data;

	sc_d->se = this->m_se;
	sc_d->stack = this->m_view_stack;
	sc_d->is_visible = this->m_search_container_visible;
	sc_d->apl = this->m_search_list;

	this->m_search_changed_data = sc_d;

	g_signal_connect(
		this->m_search_entry, "search-changed", G_CALLBACK(+[](GtkSearchEntry *entry, search_changed_data *data) {
			auto text = gtk_entry_get_text(GTK_ENTRY(entry));

			if (strlen(text) == 0) {
				(*data->is_visible) = false;
				gtk_stack_set_visible_child_name(data->stack, "list-with-category");
			} else {
				gtk_list_box_select_row(data->apl->m_app_list, gtk_list_box_get_row_at_y(data->apl->m_app_list, 0));
				gtk_list_box_invalidate_filter(data->apl->m_app_list);
				data->se->get_all_desktop_entries();
				(*data->is_visible) = true;

				gtk_stack_set_visible_child_name(data->stack, "search-list");
			}
		}),
		this->m_search_changed_data);

	g_signal_connect(this->m_search_entry, "stop-search",
					 G_CALLBACK(+[](GtkSearchEntry *entry, search_changed_data *data) {
						 gtk_stack_set_visible_child_name(data->stack, "list-with-category");
					 }),
					 this->m_search_changed_data);

	gtk_list_box_set_filter_func(
		this->m_search_list->m_app_list,
		[](GtkListBoxRow *r, void *d) -> int {
			auto sentry = gtk_entry_get_text(GTK_ENTRY(d));
			auto row_data = new app_list_row_label_data;
			auto ret_val = false;

			get_label_content_from_app_list_row(r, row_data);

			if (row_data->found_first) {
				if (ci_find_substr(row_data->title, std::string(sentry)) != -1) { ret_val = true; }
			}

			if (row_data->found_second) {
				if (ci_find_substr(row_data->desc, std::string(sentry)) != -1) { ret_val = true; }
			}

			delete row_data;
			return ret_val;
		},
		this->m_search_entry, NULL);

	g_signal_connect(tl_popover, "show", G_CALLBACK(+[](GtkWidget *w, void *data) {
						 auto se = (GtkEntry *)data;
						 gtk_widget_grab_focus(GTK_WIDGET(se));
					 }),
					 this->m_search_entry);

	// Add SearchEngine handle and search for all desktop entries

	m_se->register_result_handle([&](std::vector<se::File> files) {
		GDesktopAppInfo *app_info;
		for (auto &&file : files) {
			if (!m_se->__x && file.type == se::FileType::DesktopEntry) {
				app_info = g_desktop_app_info_new_from_filename(file.path.c_str());

				// Check app_info validity

				if (app_info == NULL) continue;
				if (g_desktop_app_info_get_nodisplay(app_info)) continue;
				if (strcmp(g_desktop_app_info_get_string(app_info, "Type"), "Application") != 0) continue;

				// Construct AppEntry

				AppEntry app_entry;

				app_entry.name = charp_to_string(g_desktop_app_info_get_string(app_info, "Name"));
				app_entry.description = charp_to_string(g_desktop_app_info_get_string(app_info, "Comment"));
				app_entry.iconstring = charp_to_string(g_desktop_app_info_get_string(app_info, "Icon"));
				app_entry.exec = charp_to_string(g_desktop_app_info_get_string(app_info, "Exec"));
				app_entry.working_path = charp_to_string(g_desktop_app_info_get_string(app_info, "Path"));

				if (app_entry.exec == "") continue;

				replace(app_entry.exec, "%F", "");
				replace(app_entry.exec, "%f", "");
				replace(app_entry.exec, "%U", "");
				replace(app_entry.exec, "%u", "");
				replace(app_entry.exec, "%i", "");
				replace(app_entry.exec, "%c", "");
				replace(app_entry.exec, "%k", "");

				this->m_search_list->add_app(app_entry);

				const char *KcP_cats = g_desktop_app_info_get_categories(app_info);

				if (KcP_cats == NULL) continue;
				auto categories_vec = split(KcP_cats, ";");

				for (auto &&category : categories_vec) {
					Category _catg;

					if (category == "AudioVideo") {
						_catg = Category::Multimedia;
					} else if (category == "Development") {
						_catg = Category::Development;
					} else if (category == "Education") {
						_catg = Category::Education;
					} else if (category == "Game") {
						_catg = Category::Game;
					} else if (category == "Graphics") {
						_catg = Category::Graphics;
					} else if (category == "Network") {
						_catg = Category::Network;
					} else if (category == "Office") {
						_catg = Category::Office;
					} else if (category == "Science") {
						_catg = Category::Science;
					} else if (category == "Settings") {
						_catg = Category::Settings;
					} else if (category == "System") {
						_catg = Category::System;
					} else if (category == "Utility") {
						_catg = Category::Accessories;
					} else {
						_catg = Category::Other;
					}

					if (!this->m_app_lists.contains(_catg)) {
						this->m_app_lists.emplace(_catg, application_list(apid, tl_popover));
						this->m_app_lists.at(_catg).add_app(app_entry);
					} else {
						this->m_app_lists.at(_catg).add_app(app_entry);
					}
				}

				if (categories_vec.size() == 0) {
					if (!this->m_app_lists.contains(Category::Other)) {
						this->m_app_lists.emplace(Category::Other, application_list(apid, tl_popover));
						this->m_app_lists.at(Category::Other).add_app(app_entry);
					} else {
						this->m_app_lists.at(Category::Other).add_app(app_entry);
					}
				}

				// Update GtkNotebook

				for (auto &[category, app_list] : this->m_app_lists) {
					if (!this->m_notebook_pages.contains(category)) {
						int pos;

						for (size_t i = 0; i < categories_tps.size(); i++) {
							if (category == categories_tps[i]) {
								pos = gtk_notebook_insert_page(
									this->m_list_container, app_list.get_widget(),
									construct_category_label(category_icon[i], category_name[i]), i);
							}
						}
						this->m_notebook_pages.emplace(category, pos);
					}
				}

				gtk_notebook_set_current_page(this->m_list_container, 0);
			}
		}
	});
	m_se->get_all_desktop_entries();

	// Add class and id for CSS

	{
		GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_list_container));
		gtk_style_context_add_class(context, "app-finder-category-notebook");
		gtk_widget_set_name(GTK_WIDGET(m_list_container),
							std::string("app-finder-category-notebook-" + std::to_string(apid)).c_str());
	}

	log_info("app-finder/list-area created");
}
list_area::~list_area() {
	delete m_se;
	delete this->m_search_changed_data;
	delete m_search_container_visible;
}

auto list_area::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }

auto list_area::back_to_defaults() -> void {
	gtk_stack_set_visible_child_name(this->m_view_stack, "list-with-category");
	gtk_entry_set_text(GTK_ENTRY(this->m_search_entry), "");
	(*this->m_search_container_visible) = false;
}
};