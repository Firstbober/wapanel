#pragma once

#include "wl_toplevel.hh"
#include <map>
#include <wlr-foreign-toplevel-management-unstable-v1-client-protocol.h>

namespace wapanel::applet::wl {

class toplevel_manager {
private:
	toplevel_manager() {};
	toplevel_manager(const toplevel_manager &) {};

	zwlr_foreign_toplevel_manager_v1 *m_manager = NULL;
	bool m_initialized = false;

	std::map<zwlr_foreign_toplevel_handle_v1 *, toplevel *> m_toplevels;

public:
	static toplevel_manager &get() {
		static toplevel_manager singleton;
		return singleton;
	}

	auto try_to_initialize() -> void;

	auto event_toplevel_new(zwlr_foreign_toplevel_handle_v1 *tl_handle) -> void;
	auto event_toplevel_finished(zwlr_foreign_toplevel_handle_v1 *tl_handle) -> void;

	auto receive_toplevel_manager(zwlr_foreign_toplevel_manager_v1 *tl_manager) -> void;
};

}