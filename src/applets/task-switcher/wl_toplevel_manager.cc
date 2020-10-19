#include "wl_toplevel_manager.hh"
#include "../../log.hh"
#include "wl_toplevel.hh"
#include <gdk/gdkwayland.h>

using tl_manager_singleton = wapanel::applet::wl::toplevel_manager;

// Wayland static handles for toplevel events

auto static zwlr_event_toplevel(void *data, zwlr_foreign_toplevel_manager_v1 *manager,
								zwlr_foreign_toplevel_handle_v1 *toplevel) -> void {
	tl_manager_singleton::get().event_toplevel_new(toplevel);
}

auto static zwlr_event_finished(void *data, zwlr_foreign_toplevel_manager_v1 *manager) -> void {}

// Wayland static handles for toplevel events

// ==========================================

// Wayland static handles for registry events

auto static registry_event_add_object(void *data, wl_registry *registry, uint32_t name, const char *interface,
									  uint32_t version) -> void {
	if (strcmp(interface, zwlr_foreign_toplevel_manager_v1_interface.name) == 0) {
		auto zwlr_toplevel_manager = (zwlr_foreign_toplevel_manager_v1 *)wl_registry_bind(
			registry, name, &zwlr_foreign_toplevel_manager_v1_interface, std::min(version, 1u));

		tl_manager_singleton::get().receive_toplevel_manager(zwlr_toplevel_manager);
	}
}
auto static registry_event_remove_object(void *data, struct wl_registry *registry, uint32_t name) -> void {}

// Wayland static handles for registry events

static zwlr_foreign_toplevel_manager_v1_listener toplevel_manager_handles = {
	.toplevel = zwlr_event_toplevel,
	.finished = zwlr_event_finished,
};

static struct wl_registry_listener toplevel_registry_listeners
	= { &registry_event_add_object, &registry_event_remove_object };

namespace wapanel::applet::wl {

auto toplevel_manager::try_to_initialize() -> void {
	if (m_initialized) return;

	auto gdk_display = gdk_display_get_default();
	auto display = gdk_wayland_display_get_wl_display(gdk_display);

	GdkSeat* seat = gdk_display_get_default_seat(gdk_display);
	m_seat = gdk_wayland_seat_get_wl_seat(seat);

	wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &toplevel_registry_listeners, NULL);
	wl_display_roundtrip(display);

	if (!this->m_manager) {
		log_error("Compositor doesn't support `wlr-foreign-toplevel-management`, exiting.");
		exit(-1);
	}

	wl_registry_destroy(registry);
	zwlr_foreign_toplevel_manager_v1_add_listener(this->m_manager, &toplevel_manager_handles, NULL);

	log_info("Initialized toplevel_manager");

	m_initialized = true;
}

auto toplevel_manager::clean() -> void {
	m_on_toplevel_new_callback.clear();
	m_on_toplevel_finished_callback.clear();

	for(auto &&[key, val] : toplevels) {
		val->clean();
	}

	log_info("Cleaned toplevel_manager");
}

auto toplevel_manager::event_toplevel_new(zwlr_foreign_toplevel_handle_v1 *tl_handle) -> void {
	toplevels[tl_handle] = new toplevel(tl_handle, toplevels.size(), m_seat);

	for (auto callback : m_on_toplevel_new_callback) {
		callback(toplevels[tl_handle]);
	}

	log_info("Adding new wl_toplevel");
}
auto toplevel_manager::event_toplevel_finished(zwlr_foreign_toplevel_handle_v1 *tl_handle) -> void {
	for (auto callback : m_on_toplevel_finished_callback) {
		callback(toplevels[tl_handle]);
	}

	delete toplevels[tl_handle];
	toplevels.erase(tl_handle);
}

auto toplevel_manager::receive_toplevel_manager(zwlr_foreign_toplevel_manager_v1 *tl_manager) -> void {
	m_manager = tl_manager;
}

auto toplevel_manager::on_toplevel_new(std::function<void(toplevel *)> callback) -> void {
	m_on_toplevel_new_callback.push_back(callback);
	log_info("Registred new callback for event `toplevel_new`");
}

auto toplevel_manager::on_toplevel_finished(std::function<void(toplevel *)> callback) -> void {
	m_on_toplevel_finished_callback.push_back(callback);
	log_info("Registred new callback for event `toplevel_finished`");
}

}