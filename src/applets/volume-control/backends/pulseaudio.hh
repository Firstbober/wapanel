#pragma once
#include "../backend.hh"
#include <pulse/pulseaudio.h>
#include <string>

namespace wapanel::applet::backends {

class pulseaudio : public backend {
private:
	pa_mainloop *m_mainloop;
	pa_mainloop_api *m_mainloop_api;
	pa_context *m_context;

	struct pa_ss_info {
		std::string name = "";
		int volume = 0;
	} pa_def_sink_info, pa_def_source_info;

public:
	pulseaudio();
	~pulseaudio();

	virtual auto initialize() -> bool;
	virtual auto destroy() -> void;

	virtual auto run() -> int;
	auto quit_main_loop(int ret) -> void;

	virtual auto get_input_volume_in_percent() -> float;
	virtual auto set_input_volume_in_percent() -> void;
	virtual auto mute_input() -> void;
	virtual auto unmute_input() -> void;

	virtual auto get_output_volume_in_percent() -> float;
	virtual auto set_output_volume_in_percent() -> void;
	virtual auto mute_output() -> void;
	virtual auto unmute_output() -> void;

	auto pa_context_state_change_callback(pa_context *ctx) -> void;
	auto pa_context_server_info_callback(pa_context *ctx, const pa_server_info *info) -> void;

	auto pa_context_sink_info_callback(pa_context *ctx, const pa_sink_info *info, int eol) -> void;
	auto pa_context_source_info_callback(pa_context *ctx, const pa_source_info *info, int eol) -> void;

private:
	static auto redirect_context_state_change_callback(pa_context *ctx, void *userdata) -> void {
		assert(ctx && userdata);

		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_state_change_callback(ctx);
	}

	static auto redirect_context_server_info_callback(pa_context *ctx, const pa_server_info *info, void *userdata)
		-> void {
		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_server_info_callback(ctx, info);
	}

	static auto redirect_context_sink_info_callback(pa_context *ctx, const pa_sink_info *info, int eol, void *userdata)
		-> void {
		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_sink_info_callback(ctx, info, eol);
	}

	static auto redirect_context_source_info_callback(pa_context *ctx, const pa_source_info *info, int eol,
													  void *userdata) -> void {
		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_source_info_callback(ctx, info, eol);
	}
};

} // namespace wapanel::applet::backends
