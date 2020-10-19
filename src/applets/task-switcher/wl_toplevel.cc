#include "wl_toplevel.hh"
#include "../../log.hh"
#include "wl_toplevel_manager.hh"
#include "wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"
#include <wayland-client-protocol.h>

// waylandpp - see 3TH_PARTY_LICENSES
#define wl_array_for_each_cpp(pos, array)                                                                              \
	for ((pos) = static_cast<decltype(pos)>((array)->data);                                                            \
		 reinterpret_cast<const char *>(pos) < (reinterpret_cast<const char *>((array)->data) + (array)->size);        \
		 (pos)++)
// waylandpp - see 3TH_PARTY_LICENSES

namespace applet_wl = wapanel::applet::wl;

// Wayland toplevel event and data handlers

auto static zwlr_toplevel_data_title(void *data, zwlr_foreign_toplevel_handle_v1 *handle, const char *title) -> void {
	auto wl_toplevel = static_cast<applet_wl::toplevel *>(data);
	wl_toplevel->title = title;

	for(auto &&callback : wl_toplevel->callbacks)
		callback(applet_wl::toplevel_event::TITLE_CHANGE);
}

auto static zwlr_toplevel_data_app_id(void *data, zwlr_foreign_toplevel_handle_v1 *handle, const char *app_id) -> void {
	auto wl_toplevel = static_cast<applet_wl::toplevel *>(data);
	wl_toplevel->app_id = app_id;
}

auto static zwlr_toplevel_data_state(void *data, zwlr_foreign_toplevel_handle_v1 *handle, wl_array *state) -> void {
	auto wl_toplevel = static_cast<applet_wl::toplevel *>(data);
	uint32_t *pos;

	wl_array_for_each_cpp(pos, state) {
		if (*pos == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED)
			wl_toplevel->state = applet_wl::toplevel_state::ACTIVATED;
		if (*pos == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MAXIMIZED)
			wl_toplevel->state = applet_wl::toplevel_state::MAXIMIZED;
		if (*pos == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MINIMIZED)
			wl_toplevel->state = applet_wl::toplevel_state::MINIMIZED;
	}
}

auto static zwlr_toplevel_event_output_enter(void *data, zwlr_foreign_toplevel_handle_v1 *handle, wl_output *output)
	-> void {
	auto wl_toplevel = static_cast<applet_wl::toplevel *>(data);
	wl_toplevel->event_output_enter(output);
}

auto static zwlr_toplevel_event_output_leave(void *data, zwlr_foreign_toplevel_handle_v1 *handle, wl_output *output)
	-> void {
	auto wl_toplevel = static_cast<applet_wl::toplevel *>(data);
	wl_toplevel->event_output_leave(output);
}

auto static zwlr_toplevel_event_done(void *data, zwlr_foreign_toplevel_handle_v1 *handle) -> void {
	auto wl_toplevel = static_cast<applet_wl::toplevel *>(data);
	wl_toplevel->event_done();
}

auto static zwlr_toplevel_event_closed(void *data, zwlr_foreign_toplevel_handle_v1 *handle) -> void {
	applet_wl::toplevel_manager::get().event_toplevel_finished(handle);
	zwlr_foreign_toplevel_handle_v1_destroy(handle);
}

static struct zwlr_foreign_toplevel_handle_v1_listener toplevel_handle_listeners
	= { .title = zwlr_toplevel_data_title,
		.app_id = zwlr_toplevel_data_app_id,

		.output_enter = zwlr_toplevel_event_output_enter,
		.output_leave = zwlr_toplevel_event_output_leave,

		.state = zwlr_toplevel_data_state,

		.done = zwlr_toplevel_event_done,
		.closed = zwlr_toplevel_event_closed };

// Wayland toplevel event handlers

// ==========================================

namespace wapanel::applet::wl {

toplevel::toplevel(zwlr_foreign_toplevel_handle_v1 *handle, unsigned int id, struct wl_seat *seat) {
	this->m_handle = handle;
	this->mgid = id;
	this->m_seat = seat;
	zwlr_foreign_toplevel_handle_v1_add_listener(handle, &toplevel_handle_listeners, this);
	log_info("Initialized wl_toplevel");
}
toplevel::~toplevel() {}

auto toplevel::clean() -> void {
	this->callbacks.clear();
}

auto toplevel::event_output_enter(wl_output *output) -> void {
	for(auto &&callback : callbacks)
		callback(toplevel_event::OUTPUT_ENTER);
}
auto toplevel::event_output_leave(wl_output *output) -> void {
	for(auto &&callback : callbacks)
		callback(toplevel_event::OUTPUT_LEAVE);
}
auto toplevel::event_done() -> void {
	if (this->state == toplevel_state::ACTIVATED) applet_wl::toplevel_manager::get().current_window = this->mgid;

	for(auto &&callback : callbacks)
		callback(toplevel_event::DONE);
}

auto toplevel::on_event(std::function<void(toplevel_event)> callback) -> void {
	this->callbacks.push_back(callback);
	log_info("Registred new event listener in wl_toplevel");
}

auto toplevel::set_maximized() -> void { zwlr_foreign_toplevel_handle_v1_set_maximized(m_handle); }
auto toplevel::unset_maximized() -> void { zwlr_foreign_toplevel_handle_v1_unset_maximized(m_handle); }

auto toplevel::set_minimized() -> void { zwlr_foreign_toplevel_handle_v1_set_minimized(m_handle); }
auto toplevel::unset_minimized() -> void { zwlr_foreign_toplevel_handle_v1_unset_minimized(m_handle); }

auto toplevel::set_activated() -> void { zwlr_foreign_toplevel_handle_v1_activate(m_handle, m_seat); }

auto toplevel::close() -> void { zwlr_foreign_toplevel_handle_v1_close(m_handle); }

}