#pragma once
#include "../../log.hh"
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace wapanel::applet {

struct menu_entry {};

enum class xdg_menu_rule_type { None, Filename, Category, All, And, Or, Not };

struct xdg_menu_rule {
	xdg_menu_rule_type type = xdg_menu_rule_type::None;
	std::vector<xdg_menu_rule> subrules;
	std::optional<std::string> optional_content;
};

struct xdg_menu {
	std::vector<xdg_menu> submenus;

	std::vector<std::filesystem::path> app_dirs;
	std::vector<std::filesystem::path> directory_dirs;

	std::string name;
	std::filesystem::path menu_directory_entry_path;
	bool only_unallocated = false;
	bool deleted = false;

	std::vector<xdg_menu_rule> include_rules;
	std::vector<xdg_menu_rule> exclude_rules;
};

class menu_entries {
public:
	static menu_entries &get() {
		static menu_entries instance;
		return instance;
	}

	std::optional<xdg_menu> root_menu;

private:
	menu_entries() {
		log_info("Spawned menu_entries singleton");
		create_cache();
	};
	~menu_entries() { log_info("Destroyed menu_entries singleton"); };
	menu_entries(const menu_entries &) = delete;
	menu_entries &operator=(const menu_entries &) = delete;

	auto create_cache() -> void;
};

} // namespace wapanel::applet
