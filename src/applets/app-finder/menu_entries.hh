#pragma once
#include "../../log.hh"
#include <string>
#include <vector>

namespace wapanel::applet {

struct menu_entry {};

struct menu_category {
	std::string name;
	std::vector<menu_entry> entries;
};

class menu_entries {
public:
	static menu_entries &get() {
		static menu_entries instance;
		return instance;
	}

	auto get_categories_names() -> std::vector<std::string>;

private:
	menu_entries() {
		log_info("Spawned menu_entries singleton");
		create_cache();
	};
	~menu_entries() { log_info("Destroyed menu_entries singleton"); };
	menu_entries(const menu_entries &) = delete;
	menu_entries &operator=(const menu_entries &) = delete;

	auto create_cache() -> void;

	std::vector<menu_category> m_categories;
};

} // namespace wapanel::applet
