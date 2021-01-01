#include "menu_entries.hh"
#include <cstdlib>
#include <pugixml.hpp>
#include <string.h>
#include <xdgpp/xdg.hpp>

namespace wapanel::applet {

auto parse_rules(pugi::xml_node rule_node) -> std::optional<std::vector<xdg_menu_rule>> {
	std::vector<xdg_menu_rule> rules;

	for (auto &&child_node : rule_node.children()) {
		std::optional<xdg_menu_rule> new_rule;
		xdg_menu_rule tmp_rule;

		if (strcmp(child_node.name(), "Filename") == 0) {
			if (child_node.text()) {
				tmp_rule.type = xdg_menu_rule_type::Filename;
				tmp_rule.optional_content = child_node.text().as_string();

				new_rule = tmp_rule;
			}
		}

		if (strcmp(child_node.name(), "Category") == 0) {
			if (child_node.text()) {
				tmp_rule.type = xdg_menu_rule_type::Category;
				tmp_rule.optional_content = child_node.text().as_string();

				new_rule = tmp_rule;
			}
		}

		if (strcmp(child_node.name(), "All") == 0) {
			tmp_rule.type = xdg_menu_rule_type::All;
			new_rule = tmp_rule;
		}

		if (strcmp(child_node.name(), "And") == 0) {
			auto sub_rules = parse_rules(child_node);
			if (sub_rules) {
				tmp_rule.type = xdg_menu_rule_type::And;
				tmp_rule.subrules = sub_rules.value();

				new_rule = tmp_rule;
			}
		}

		if (strcmp(child_node.name(), "Or") == 0) {
			auto sub_rules = parse_rules(child_node);
			if (sub_rules) {
				tmp_rule.type = xdg_menu_rule_type::Or;
				tmp_rule.subrules = sub_rules.value();

				new_rule = tmp_rule;
			}
		}

		if (strcmp(child_node.name(), "Not") == 0) {
			auto sub_rules = parse_rules(child_node);
			if (sub_rules) {
				tmp_rule.type = xdg_menu_rule_type::Not;
				tmp_rule.subrules = sub_rules.value();

				new_rule = tmp_rule;
			}
		}

		if (new_rule) rules.push_back(new_rule.value());
	}

	if (rules.size() == 0) {
		return {};
	} else {
		return rules;
	}
}

auto parse_menu_node(pugi::xml_node menu_node) -> std::optional<xdg_menu> {
	xdg_menu menu;

	for (auto &&child_node : menu_node.children()) {
		log_warn("%s - %s", child_node.name(), child_node.text().as_string());

		if (strcmp(child_node.name(), "Menu") == 0) {
			auto submenu = parse_menu_node(child_node);
			if (submenu) menu.submenus.push_back(submenu.value());
		}

		if (strcmp(child_node.name(), "AppDir") == 0)
			if (child_node.text()) menu.app_dirs.push_back(std::filesystem::path(child_node.text().as_string()));

		if (strcmp(child_node.name(), "DirectoryDir") == 0)
			if (child_node.text()) menu.directory_dirs.push_back(std::filesystem::path(child_node.text().as_string()));

		if (strcmp(child_node.name(), "Name") == 0 && menu.name != "") {
			if (child_node.text()) {
				const pugi::char_t *name = child_node.text().as_string();
				bool contain_slash = false;

				for (size_t i = 0; i < strlen(name); i++) {
					if (name[i] == '/') contain_slash = true;
				}

				if (!contain_slash) menu.name = child_node.text().as_string();
			}
		}

		if (strcmp(child_node.name(), "Directory") == 0)
			if (child_node.text()) menu.menu_directory_entry_path = child_node.text().as_string();

		if (strcmp(child_node.name(), "OnlyUnallocated") == 0) menu.only_unallocated = true;

		if (strcmp(child_node.name(), "NotOnlyUnallocated") == 0) menu.only_unallocated = false;

		if (strcmp(child_node.name(), "Deleted") == 0) menu.deleted = true;

		if (strcmp(child_node.name(), "NotDeleted") == 0) menu.deleted = false;

		if (strcmp(child_node.name(), "Include") == 0) {
			auto new_rule = parse_rules(child_node);
			if (new_rule) {
				menu.include_rules.insert(menu.include_rules.end(), new_rule.value().begin(), new_rule.value().end());
			}
		}

		if (strcmp(child_node.name(), "Exclude") == 0) {
			auto new_rule = parse_rules(child_node);
			if (new_rule) {
				menu.exclude_rules.insert(menu.exclude_rules.end(), new_rule.value().begin(), new_rule.value().end());
			}
		}
	}

	// ---- <Menu> independent nodes

	if (menu_node.child("DefaultAppDirs")) {
		for (auto &&data_dir : xdg::DataDirs()) {
			std::filesystem::path app_dir = data_dir;
			app_dir += "/applications";

			menu.app_dirs.push_back(app_dir);
		}
	}

	if (menu_node.child("DefaultDirectoryDirs")) {
		for (auto &&data_dir : xdg::DataDirs()) {
			std::filesystem::path directory_dir = data_dir;
			directory_dir += "/desktop-directories";

			menu.directory_dirs.push_back(directory_dir);
		}
	}

	// ++++ <Menu> independent nodes

	if (!menu.deleted) {
		return menu;
	} else {
		return {};
	}

	// TODO: Add XML nodes from menu spec after the "<Not>" one
}

auto menu_entries::create_cache() -> void {
	log_info("Creating entry cache");

	char *xdg_menu_prefix = std::getenv("XDG_MENU_PREFIX");
	std::string application_menu_file_name = "";
	std::filesystem::path used_menu_file_path;

	if (xdg_menu_prefix != NULL) {
		application_menu_file_name = std::string(xdg_menu_prefix) + std::string("applications.menu");
	} else {
		application_menu_file_name = "applications.menu";
	}

	for (auto &&xdg_path : xdg::ConfigDirs()) {
		if (std::filesystem::exists(xdg_path.generic_string() + "/menus/" + application_menu_file_name)) {
			used_menu_file_path = xdg_path.generic_string() + "/menus/" + application_menu_file_name;
			break;
		}
		if (!std::filesystem::exists(xdg_path.generic_string() + "/menus")) {
			log_warn("`%s` is non existent", std::string(xdg_path.generic_string() + "/menus").c_str());
			continue;
		}

		std::filesystem::path menus_path = xdg_path.generic_string() + "/menus";
		for (auto &&dir_entry : std::filesystem::directory_iterator(menus_path)) {
			used_menu_file_path = dir_entry;
			break;
		}
	}

	if (used_menu_file_path.empty()) {
		log_error("No menu file was found.");
		return;
	}

	log_info("Found menu file `%s`", used_menu_file_path.c_str());

	pugi::xml_document menu_document;
	pugi::xml_parse_result parsing_result = menu_document.load_file(used_menu_file_path.c_str());

	if (!parsing_result) {
		log_error("Found some errors while parsing menu file:");
		log_error("Descritpion: %s", parsing_result.description());
		log_error("Offset: %td", parsing_result.offset);
		return;
	} else {
		log_info("Parsed menu file successfully");
	}

	pugi::xml_node root = menu_document.child("Menu");
	if (!root) {
		log_error("Cannot find root node in menu file");
		return;
	}

	this->root_menu = parse_menu_node(root);
	if (!root_menu) log_error("Parsed menu is deleted, nothing will be shown");

	// TODO: Make processing of parsed data
	// Merging, rule execution etc.
}

} // namespace wapanel::applet
