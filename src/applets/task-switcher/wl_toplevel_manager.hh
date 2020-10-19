#pragma once

#include "wl_toplevel.hh"
#include <functional>
#include <unordered_map>
#include <vector>
#include <wayland-client-protocol.h>
#include <wlr-foreign-toplevel-management-unstable-v1-client-protocol.h>

namespace wapanel::applet::wl {

class toplevel_manager {
private:
	toplevel_manager() {};
	toplevel_manager(const toplevel_manager &) {};

	zwlr_foreign_toplevel_manager_v1 *m_manager = NULL;
	bool m_initialized = false;

	struct wl_seat* m_seat;

	std::vector<std::function<void(toplevel *)>> m_on_toplevel_new_callback;
	std::vector<std::function<void(toplevel *)>> m_on_toplevel_finished_callback;

public:
	static toplevel_manager &get() {
		static toplevel_manager singleton;
		return singleton;
	}

	std::unordered_map<zwlr_foreign_toplevel_handle_v1 *, toplevel *> toplevels;
	unsigned int current_window;

	auto try_to_initialize() -> void;
	auto clean() -> void;

	auto event_toplevel_new(zwlr_foreign_toplevel_handle_v1 *tl_handle) -> void;
	auto event_toplevel_finished(zwlr_foreign_toplevel_handle_v1 *tl_handle) -> void;

	auto receive_toplevel_manager(zwlr_foreign_toplevel_manager_v1 *tl_manager) -> void;

	auto on_toplevel_new(std::function<void(toplevel *)> callback) -> void;
	auto on_toplevel_finished(std::function<void(toplevel *)> callback) -> void;
};

}