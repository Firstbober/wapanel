#include "menu_entries.hh"
#include <cstdlib>
#include <filesystem>
#include <xdgpp/xdg.hpp>

namespace wapanel::applet {

auto menu_entries::get_categories_names() -> std::vector<std::string> {}

auto menu_entries::create_cache() -> void {
	log_info("Creating entry cache");

	char *xdg_menu_prefix = std::getenv("XDG_MENU_PREFIX");
	std::string application_menu_file_name = "";
	std::filesystem::path used_menu_file_path;

	if (xdg_menu_prefix != NULL)
		application_menu_file_name = std::string(xdg_menu_prefix) + std::string("applications.menu");

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

	if(used_menu_file_path.empty()) {
		log_error("No menu file was found.");
		return;
	}

	log_info("Found menu file `%s`", used_menu_file_path.c_str());

	// TODO: Add all XML stuff here
}

} // namespace wapanel::applet
