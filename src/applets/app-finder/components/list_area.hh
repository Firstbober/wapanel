#pragma once
#include "../app-entry.hh"
#include "../search-engine/search_engine.hh"
#include "search-engine/search_engine_mime.hh"
/*
#include <concurrentqueue/blockingconcurrentqueue.h>
#include <condition_variable>
*/

#include <cstddef>
#include <gtk/gtk.h>
#include <map>
#include <string>
//#include <thread>
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

/*
class search_container {

private:
	GtkScrolledWindow *m_scroll_win;
	GtkViewport *m_viewport;
	GtkListBox *m_app_list;

public:
	struct SearchEntry {
		std::string title;
		std::string iconstring;
		std::string path;
		std::string working_path;

		se::FileType type;
	};

public:
	search_container(int apid, GtkPopover *tl_popover);

	auto get_widget() -> GtkWidget *;
	auto add_entry(SearchEntry search_entry) -> void;
	auto reset() -> void;
	auto show_all() -> void;

	// moodycamel::ConcurrentQueue<search_container::SearchEntry, > m_widgets_to_add;
private:
};
*/

class list_area {
private:
	GtkBox *m_root;
	GtkNotebook *m_list_container;
	GtkNotebook *m_search_container;
	GtkStack *m_view_stack;
	GtkSearchEntry *m_search_entry;

	bool m_search_container_visible = false;

	se::SearchEngine *m_se;

	std::map<Category, application_list> m_app_lists;
	std::map<Category, int> m_notebook_pages;

	/*
	enum class SearchPageType { Apps, Directories, Multimedia, Docs };
	std::map<SearchPageType, search_container> m_search_containers;

	moodycamel::BlockingConcurrentQueue<search_container::SearchEntry> m_search_queue;
	std::thread m_update_search_container_th;
	std::condition_variable m_container_update_notifier;
	std::mutex m_cun_mutex;
	bool m_is_update_th_active = true;
	bool m_needs_update = true;

	struct search_changed_data {
		se::SearchEngine *se;
		GtkStack *stack;
		bool *needs_update;
		moodycamel::BlockingConcurrentQueue<search_container::SearchEntry> *search_queue;
		std::map<SearchPageType, search_container> *sces;
		moodycamel::BlockingConcurrentQueue<search_container::SearchEntry*> *search_entries_to_clear;
	} * m_search_changed_data;

	moodycamel::BlockingConcurrentQueue<search_container::SearchEntry*> m_search_entries_to_clear;
	*/
public:
	/*
		struct search_update_data {
			search_container *sce;
			search_container::SearchEntry *see;
		};
		*/

	list_area(int apid, GtkPopover *tl_popover);
	~list_area();

	auto get_widget() -> GtkWidget *;

private:
	// moodycamel::BlockingConcurrentQueue<search_update_data *> m_sud_pointers;
};

};