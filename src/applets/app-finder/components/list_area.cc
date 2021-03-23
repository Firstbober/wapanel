#include "list_area.hh"
#include "../../icon_cache.hh"
#include "app-entry.hh"
#include "search-engine/search_engine.hh"
#include "search-engine/search_engine_mime.hh"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <gio/gdesktopappinfo.h>
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

application_list::application_list(int apid, GtkPopover *tl_popover)
	: m_scroll_win(GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL)))
	, m_viewport(GTK_VIEWPORT(gtk_viewport_new(NULL, NULL)))
	, m_app_list(GTK_LIST_BOX(gtk_list_box_new())) {
	gtk_list_box_set_selection_mode(m_app_list, GTK_SELECTION_NONE);
	gtk_list_box_set_activate_on_single_click(m_app_list, true);

	gtk_container_add(GTK_CONTAINER(m_scroll_win), GTK_WIDGET(m_viewport));
	gtk_container_add(GTK_CONTAINER(m_viewport), GTK_WIDGET(m_app_list));

	gtk_widget_show_all(GTK_WIDGET(m_scroll_win));

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

// Search container
/*
search_container::search_container(int apid, GtkPopover *tl_popover)
	: m_scroll_win(GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL)))
	, m_viewport(GTK_VIEWPORT(gtk_viewport_new(NULL, NULL)))
	, m_app_list(GTK_LIST_BOX(gtk_list_box_new())) {
	gtk_list_box_set_selection_mode(m_app_list, GTK_SELECTION_NONE);
	gtk_list_box_set_activate_on_single_click(m_app_list, true);

	gtk_container_add(GTK_CONTAINER(m_scroll_win), GTK_WIDGET(m_viewport));
	gtk_container_add(GTK_CONTAINER(m_viewport), GTK_WIDGET(m_app_list));

	gtk_widget_show_all(GTK_WIDGET(m_scroll_win));

	// Styling

	GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_scroll_win));
	gtk_style_context_add_class(context, "app-finder-search-app-list");
	gtk_widget_set_name(GTK_WIDGET(m_scroll_win),
						std::string("app-finder-search-app-list-" + std::to_string(apid)).c_str());
}

auto search_container::get_widget() -> GtkWidget * { return GTK_WIDGET(m_scroll_win); }
auto search_container::add_entry(SearchEntry search_entry) -> void {
	if (search_entry.working_path.empty()) { search_entry.working_path = "."; }
	if (search_entry.title.empty()) { search_entry.title = ""; }

	// m_widgets_to_add.enqueue(search_entry);

	GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6));

	constexpr int icon_size = 20;
	gtk_box_pack_start(
		box, gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon(search_entry.iconstring, icon_size)), false,
		true, 0);

	GtkLabel *title = GTK_LABEL(gtk_label_new(search_entry.title.c_str()));
	gtk_label_set_ellipsize(title, PANGO_ELLIPSIZE_END);
	gtk_widget_set_halign(GTK_WIDGET(title), GTK_ALIGN_START);

	gtk_box_pack_start(box, GTK_WIDGET(title), false, false, 0);

	gtk_widget_set_margin_top(GTK_WIDGET(box), 2);
	gtk_widget_set_margin_bottom(GTK_WIDGET(box), 2);
	gtk_widget_set_margin_start(GTK_WIDGET(box), 4);
	gtk_widget_set_margin_end(GTK_WIDGET(box), 4);

	gtk_widget_show_all(GTK_WIDGET(box));
	
	char *cpy = (char *)malloc(search_entry.path.length() + 1);
	strncpy(cpy, search_entry.path.c_str(), search_entry.path.length() + 1);
	g_object_set_data(G_OBJECT(box), "_path", cpy);

	cpy = (char *)malloc(search_entry.working_path.length() + 1);
	strncpy(cpy, search_entry.working_path.c_str(), search_entry.working_path.length() + 1);
	g_object_set_data(G_OBJECT(box), "_working_path", cpy);

	g_object_set_data(G_OBJECT(box), "_type", &search_entry.type);

	gtk_widget_set_tooltip_text(GTK_WIDGET(box), search_entry.title.c_str());
	

	gtk_list_box_insert(m_app_list, GTK_WIDGET(box), -1);
	// m_widgets_to_add.enqueue(GTK_WIDGET(box));
}

auto search_container::reset() -> void {
	// gtk_widget_destroy(GTK_WIDGET(m_app_list));
	// m_app_list = GTK_LIST_BOX(gtk_list_box_new());
	gtk_container_foreach(
		GTK_CONTAINER(m_app_list),
		[](GtkWidget *widget, gpointer data) { gtk_container_remove(GTK_CONTAINER(data), widget); }, m_app_list);
}

auto search_container::show_all() -> void {}
*/

// List area

/*
static auto insert_to_search_container(gpointer user_data) -> gboolean {
	struct list_area::search_update_data *sud = (list_area::search_update_data *)user_data;
	sud->sce->add_entry(*sud->see);

	return G_SOURCE_REMOVE;
}
*/

list_area::list_area(int apid, GtkPopover *tl_popover)
	: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6)))
	, m_list_container(GTK_NOTEBOOK(gtk_notebook_new()))
	, m_search_container(GTK_NOTEBOOK(gtk_notebook_new()))
	, m_view_stack(GTK_STACK(gtk_stack_new()))
	, m_search_entry(GTK_SEARCH_ENTRY(gtk_search_entry_new()))
	, m_se(new se::SearchEngine()) {

	// Initialize category notebook

	gtk_notebook_set_tab_pos(this->m_list_container, GTK_POS_RIGHT);
	gtk_stack_add_named(this->m_view_stack, GTK_WIDGET(this->m_list_container), "list-with-category");

	// Initialize search notebook

	//gtk_stack_add_named(this->m_view_stack, GTK_WIDGET(this->m_search_container), "search-notebook");

	// Configure stack and search entry

	gtk_stack_set_visible_child_name(this->m_view_stack, "list-with-category");

	//gtk_stack_set_visible_child_name(this->m_view_stack, "search-notebook");
	//gtk_entry_set_placeholder_text(GTK_ENTRY(this->m_search_entry), "Search...");

	// Pack everything to m_root and show all

	//gtk_box_pack_end(this->m_root, GTK_WIDGET(this->m_search_entry), false, false, 0);
	gtk_box_pack_end(this->m_root, GTK_WIDGET(this->m_view_stack), true, true, 0);

	gtk_widget_show_all(GTK_WIDGET(this->m_root));

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
			/*
			if (m_se->__x) {
				search_container::SearchEntry see;

				see.title = "";
				see.iconstring = "application-x-executable-symbolic";
				see.path = file.path;
				see.working_path = "";

				see.type = file.type;

				if (m_search_queue.size_approx() >= 500 && file.type != se::FileType::DesktopEntry) { continue; }

				switch (file.type) {
				case se::FileType::DesktopEntry:
					app_info = g_desktop_app_info_new_from_filename(file.path.c_str());

					// Check app_info validity

					if (app_info == NULL) continue;
					if (g_desktop_app_info_get_nodisplay(app_info)) continue;
					if (strcmp(g_desktop_app_info_get_string(app_info, "Type"), "Application") != 0) continue;

					// Insert some data to SearchEntry

					see.title = charp_to_string(g_desktop_app_info_get_string(app_info, "Name"));
					see.path = charp_to_string(g_desktop_app_info_get_string(app_info, "Exec"));
					see.working_path = charp_to_string(g_desktop_app_info_get_string(app_info, "Path"));
					see.iconstring = charp_to_string(g_desktop_app_info_get_string(app_info, "Icon"));
					break;
				case se::FileType::Directory:
					see.iconstring = "folder-symbolic";
				case se::FileType::Document:
					see.iconstring = "text-x-generic-symbolic";
					break;
				case se::FileType::Audio:
					see.iconstring = "audio-x-generic-symbolic";
					break;
				case se::FileType::Video:
					see.iconstring = "video-x-generic-symbolic";
					break;
				case se::FileType::Image:
					see.iconstring = "image-x-generic-symbolic";
					break;
				default:
					see.iconstring = "application-x-executable-symbolic";
					break;
				}

				if (see.title.empty()) { see.title = split(see.path, "/").back(); }

				m_search_queue.enqueue(see);
			}
			*/
		}
		// if (m_se->__x) m_container_update_notifier.notify_all();
	});
	m_se->get_all_desktop_entries();
	/*
		m_update_search_container_th = std::thread([&]() {
			while (this->m_is_update_th_active) {
				std::unique_lock<std::mutex> lk(this->m_cun_mutex);
				this->m_container_update_notifier.wait(lk);

				search_container::SearchEntry see;
				while (m_search_queue.try_dequeue(see)) {
					if (!m_needs_update) break;

					log_error("%s", see.title.c_str());

					switch (see.type) {
					case se::FileType::DesktopEntry:
						if (!m_search_containers.contains(SearchPageType::Apps)) {
							auto sce = search_container(apid, tl_popover);
							m_search_containers.emplace(SearchPageType::Apps, sce);

							auto pp_see = new search_container::SearchEntry(see);
							m_search_entries_to_clear.enqueue(pp_see);

							auto sud = new search_update_data { &sce, &see };
							m_sud_pointers.enqueue(sud);

							gdk_threads_add_idle(insert_to_search_container, sud);

							gtk_notebook_insert_page(this->m_search_container, sce.get_widget(), gtk_label_new("Apps"),
	   0); } else { auto pp_see = new search_container::SearchEntry(see); m_search_entries_to_clear.enqueue(pp_see);

							auto sud = new search_update_data { &m_search_containers.at(SearchPageType::Apps), pp_see };
							m_sud_pointers.enqueue(sud);

							gdk_threads_add_idle(insert_to_search_container, sud);
						}
						break;

					case se::FileType::Directory:
						if (!m_search_containers.contains(SearchPageType::Directories)) {
							auto sce = search_container(apid, tl_popover);
							m_search_containers.emplace(SearchPageType::Directories, sce);

							see.title = see.path;
							auto pp_see = new search_container::SearchEntry(see);
							m_search_entries_to_clear.enqueue(pp_see);

							auto sud = new search_update_data { &sce, &see };
							m_sud_pointers.enqueue(sud);

							gdk_threads_add_idle(insert_to_search_container, sud);

							gtk_notebook_insert_page(this->m_search_container, sce.get_widget(),
													 gtk_label_new("Directories"), 1);
						} else {
							auto pp_see = new search_container::SearchEntry(see);
							m_search_entries_to_clear.enqueue(pp_see);

							auto sud
								= new search_update_data { &m_search_containers.at(SearchPageType::Directories), pp_see
	   }; m_sud_pointers.enqueue(sud);

							gdk_threads_add_idle(insert_to_search_container, sud);
						}
						break;

					case se::FileType::Audio:
					case se::FileType::Video:
					case se::FileType::Image:
						if (!m_search_containers.contains(SearchPageType::Multimedia)) {
							auto sce = search_container(apid, tl_popover);
							m_search_containers.emplace(SearchPageType::Multimedia, sce);

							auto pp_see = new search_container::SearchEntry(see);
							m_search_entries_to_clear.enqueue(pp_see);

							auto sud = new search_update_data { &sce, &see };
							m_sud_pointers.enqueue(sud);

							gdk_threads_add_idle(insert_to_search_container, sud);

							gtk_notebook_insert_page(this->m_search_container, sce.get_widget(),
													 gtk_label_new("Multimedia"), 2);
						} else {
							auto pp_see = new search_container::SearchEntry(see);
							m_search_entries_to_clear.enqueue(pp_see);

							auto sud
								= new search_update_data { &m_search_containers.at(SearchPageType::Multimedia), pp_see
	   }; m_sud_pointers.enqueue(sud);

							gdk_threads_add_idle(insert_to_search_container, sud);
						}
						break;

					case se::FileType::Document:
						if (!m_search_containers.contains(SearchPageType::Docs)) {
							auto sce = search_container(apid, tl_popover);
							m_search_containers.emplace(SearchPageType::Docs, sce);

							auto pp_see = new search_container::SearchEntry(see);
							m_search_entries_to_clear.enqueue(pp_see);

							auto sud = new search_update_data { &sce, &see };
							m_sud_pointers.enqueue(sud);

							gdk_threads_add_idle(insert_to_search_container, sud);

							gtk_notebook_insert_page(this->m_search_container, sce.get_widget(),
	   gtk_label_new("Documents"), 3); } else { auto pp_see = new search_container::SearchEntry(see);
							m_search_entries_to_clear.enqueue(pp_see);

							auto sud = new search_update_data { &m_search_containers.at(SearchPageType::Docs), pp_see };
							m_sud_pointers.enqueue(sud);

							gdk_threads_add_idle(insert_to_search_container, sud);
						}
						break;

					default:
						break;
					}

					gtk_notebook_set_current_page(this->m_search_container, 0);
				}

				for (auto &[x, sce] : m_search_containers) {
					sce.show_all();
				}
			}
		});
	*/
	// Connect search-changed to GtkSearchEntry
	/*
	m_search_changed_data = new search_changed_data {
		m_se, m_view_stack, &m_needs_update, &m_search_queue, &m_search_containers, &m_search_entries_to_clear
	};

	g_signal_connect(m_search_entry, "search-changed", G_CALLBACK(+[](GtkSearchEntry *entry, search_changed_data *scd) {
						 scd->se->__x = true;
						 if (gtk_entry_get_text_length(GTK_ENTRY(entry)) > 0) {
							 *scd->needs_update = false;

							 scd->se->stop_search();

							 search_container::SearchEntry see;
							 while (scd->search_queue->try_dequeue(see)) {}

							 for (auto &[cat, sce] : *scd->sces) {
								 sce.reset();
							 }

							 search_container::SearchEntry *pp_see;
							 while (scd->search_entries_to_clear->try_dequeue(pp_see)) {
								 free(pp_see);
							 }

							 *scd->needs_update = true;
							 scd->se->search(gtk_entry_get_text(GTK_ENTRY(entry)));
							 gtk_stack_set_visible_child_name(scd->stack, "search-notebook");
						 } else {
							 // gtk_stack_set_visible_child_name(scd->stack, "list-with-category");
							 scd->se->__x = false;
							 *scd->needs_update = false;
							 scd->se->stop_search();

							 for (auto &[cat, sce] : *scd->sces) {
								 sce.reset();
							 }

							 search_container::SearchEntry see;
							 while (scd->search_queue->try_dequeue(see)) {}
						 }
					 }),
					 m_search_changed_data);
	*/
	// Add class and id for CSS

	{
		GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(m_list_container));
		gtk_style_context_add_class(context, "app-finder-category-notebook");
		gtk_widget_set_name(GTK_WIDGET(m_list_container),
							std::string("app-finder-category-notebook-" + std::to_string(apid)).c_str());

		/*
				context = gtk_widget_get_style_context(GTK_WIDGET(m_search_container));
				gtk_style_context_add_class(context, "app-finder-search-notebook");
				gtk_widget_set_name(GTK_WIDGET(m_search_container),
									std::string("app-finder-search-notebook-" + std::to_string(apid)).c_str());
			*/
	}

	log_info("app-finder/list-area created");
}
list_area::~list_area() {
	/*
	search_container::SearchEntry see;
	while (m_search_queue.try_dequeue(see)) {}

	void *sud;
	while (m_sud_pointers.try_dequeue(sud)) {
		free(sud);
	}

	m_is_update_th_active = false;
	m_container_update_notifier.notify_all();
	m_container_update_notifier.notify_all();
	if (m_update_search_container_th.joinable()) m_update_search_container_th.join();

	delete m_search_changed_data;
	*/
	delete m_se;
}

auto list_area::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }
};