#include "icon_cache.hh"
#include "../../config_data.hh"

namespace wapanel::applet::utils::ic {

std::unordered_map<int, icon_cache *> _sizes;

icon_cache::icon_cache() {
	default_icon_theme = gtk_icon_theme_get_default();
	gtk_icon_theme_append_search_path(default_icon_theme, PREFIX_DATA_DIR "/icons");
}

icon_cache::~icon_cache() {
	for (auto &&[key, val] : icons) {
		g_object_unref(val);
	}

	icons.clear();
}

auto icon_cache::get_icon(std::string icon_name, int icon_size) -> GdkPixbuf * {
	if (icons.contains(icon_name)) {
		return icons[icon_name];
	} else {
		GdkPixbuf *icon = gtk_icon_theme_load_icon(default_icon_theme, icon_name.c_str(), icon_size,
												   GTK_ICON_LOOKUP_FORCE_SIZE, NULL);
		if (icon == NULL) {
			log_error("Cannot find icon `%s`", icon_name.c_str());
			return NULL;
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
