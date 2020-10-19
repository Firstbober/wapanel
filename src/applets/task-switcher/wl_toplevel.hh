#pragma once

#include <string>
#include <wlr-foreign-toplevel-management-unstable-v1-client-protocol.h>
#include <functional>
#include <vector>

namespace wapanel::applet::wl {

enum class toplevel_state { ACTIVATED = 0, MAXIMIZED = 1, MINIMIZED = 2 };
enum class toplevel_event { DONE = 0, OUTPUT_ENTER = 1, OUTPUT_LEAVE = 2, TITLE_CHANGE = 4 };

struct toplevel {
	std::string title;
	std::string app_id;
	toplevel_state state;
	unsigned int mgid;

	std::vector<std::function<void(toplevel_event)>> callbacks;

	toplevel(zwlr_foreign_toplevel_handle_v1 *handle, unsigned int id, struct wl_seat* seat);
	~toplevel();

	auto clean() -> void;

	auto event_output_enter(wl_output *output) -> void;
	auto event_output_leave(wl_output *output) -> void;
	auto event_done() -> void;

	auto on_event(std::function<void(toplevel_event)> callback) -> void;

	auto set_maximized() -> void;
	auto unset_maximized() -> void;

	auto set_minimized() -> void;
	auto unset_minimized() -> void;

	auto set_activated() -> void;

	auto close() -> void;

private:
	zwlr_foreign_toplevel_handle_v1 *m_handle;
	struct wl_seat* m_seat;
};

}