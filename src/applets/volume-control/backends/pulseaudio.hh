#pragma once
#include "../backend.hh"
#include <pulse/pulseaudio.h>

namespace wapanel::applet::backends {

class pulseaudio : public backend {
private:
	pa_mainloop *m_mainloop;
	pa_mainloop_api *m_mainloop_api;
	pa_context *m_context;

public:
	pulseaudio();
	~pulseaudio();

	virtual auto initialize() -> bool;
	virtual auto destroy() -> void;

	virtual auto get_input_volume_in_percent() -> int;
	virtual auto set_input_volume_in_percent() -> void;
	virtual auto mute_input() -> void;
	virtual auto unmute_input() -> void;

	virtual auto get_output_volume_in_percent() -> int;
	virtual auto set_output_volume_in_percent() -> void;
	virtual auto mute_output() -> void;
	virtual auto unmute_output() -> void;

	auto pa_context_state_change_callback(pa_context *ctx) -> void;

private:
	static auto ex_context_state_change_callback(pa_context *ctx, void *userdata) -> void {
		assert(ctx && userdata);

		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_state_change_callback(ctx);
	}
};

} // namespace wapanel::applet::backends
