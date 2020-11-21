#pragma once
#include "../../log.hh"
#include <gtk/gtk.h>
#include <string>
#include <unordered_map>

namespace wapanel::applet::ic {

struct icon_cache {
	std::unordered_map<std::string, GdkPixbuf *> icons;

	icon_cache() {}
	~icon_cache() {
		for (auto &&[key, val] : icons) {
			g_object_unref(val);
		}

		icons.clear();
	}

	auto get_icon(std::string icon_name, int icon_size) -> GdkPixbuf *;
};

extern std::unordered_map<int, icon_cache *> _sizes;

auto get_icon(std::string icon_name, int icon_size) -> GdkPixbuf *;
auto clean() -> void;

} // namespace wapanel::applet
