#include "icon_cache.hh"
#include "../../config_data.hh"

namespace wapanel::applet::utils::ic {

std::unordered_map<int, icon_cache *> _sizes;

icon_cache::icon_cache() {
	default_icon_theme = gtk_icon_theme_get_for_screen(gdk_screen_get_default());
	gtk_icon_theme_append_search_path(default_icon_theme, APP_DATA_DIR "/icons");
}

icon_cache::~icon_cache() {
	for (auto &&[key, val] : icons) {
		g_object_unref(val);
	}

	icons.clear();
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

auto icon_cache::get_icon(std::string icon_name, int icon_size) -> GdkPixbuf * {
	if (icons.contains(icon_name)) {
		return icons[icon_name];
	} else {
		bool load_file = false;
		if (icon_name.length() > 0) {
			if (icon_name[0] == '/') { load_file = true; }
		}

		GdkPixbuf *icon;

		if (load_file) {
			icon = gdk_pixbuf_new_from_file_at_size(icon_name.c_str(), icon_size, icon_size, NULL);
		} else {
			icon = gtk_icon_theme_load_icon(default_icon_theme, icon_name.c_str(), icon_size,
											GTK_ICON_LOOKUP_FORCE_SIZE, NULL);
		}

		if (icon == NULL) {
			auto splt = split(icon_name, ".");
			if (splt.size() > 0) {
				icon = gtk_icon_theme_load_icon(default_icon_theme, splt[0].c_str(), icon_size,
												GTK_ICON_LOOKUP_FORCE_SIZE, NULL);
			}
		}

		if (icon == NULL) {
			log_error("Cannot find icon `%s`", icon_name.c_str());

			icon = gtk_icon_theme_load_icon(default_icon_theme, "application-x-executable", icon_size,
											GTK_ICON_LOOKUP_FORCE_SIZE, NULL);

			return icon;
		}

		log_info("Initialized cache for icon `%s` with size `%d`", icon_name.c_str(), icon_size);

		icons[icon_name] = icon;
		return icons[icon_name];
	}
}

auto get_icon(std::string icon_name, int icon_size) -> GdkPixbuf * {
	if (!_sizes.contains(icon_size)) { _sizes[icon_size] = new icon_cache(); }

	return _sizes[icon_size]->get_icon(icon_name, icon_size);
}

auto clean() -> void {
	for (auto &&[key, val] : _sizes) {
		delete val;
	}

	_sizes.clear();
}

} // namespace wapanel::applet
