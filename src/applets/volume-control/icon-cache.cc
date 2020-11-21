#include "icon-cache.hh"

namespace wapanel::applet::ic {

std::unordered_map<int, icon_cache *> _sizes;

auto icon_cache::get_icon(std::string icon_name, int icon_size) -> GdkPixbuf * {
	if (icons.contains(icon_name)) {
		return icons[icon_name];
	} else {
		GtkIconTheme *default_icon_theme = gtk_icon_theme_get_default();
		icons[icon_name] = gtk_icon_theme_load_icon(default_icon_theme, icon_name.c_str(), icon_size,
													GTK_ICON_LOOKUP_FORCE_SYMBOLIC, NULL);

		log_info("Initialized cache for icon `%s` with size `%d`", icon_name.c_str(), icon_size);
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
