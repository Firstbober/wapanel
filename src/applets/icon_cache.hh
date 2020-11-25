#pragma once
#include "../log.hh"
#include <gtk/gtk.h>
#include <string>
#include <unordered_map>

namespace wapanel::applet::utils::ic {

struct icon_cache {
	std::unordered_map<std::string, GdkPixbuf *> icons;
	GtkIconTheme *default_icon_theme;

	icon_cache();
	~icon_cache();

	auto get_icon(std::string icon_name, int icon_size) -> GdkPixbuf *;
};

extern std::unordered_map<int, icon_cache *> _sizes;

auto get_icon(std::string icon_name, int icon_size) -> GdkPixbuf *;
auto clean() -> void;

} // namespace wapanel::applet
