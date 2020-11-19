#pragma once
#include "../backend.hh"
#include <pulse/pulseaudio.h>
#include <string>
#include <vector>

namespace wapanel::applet::backends {

class pulseaudio : public backend {
private:
	pa_mainloop *m_mainloop;
	pa_mainloop_api *m_mainloop_api;
	pa_context *m_context;

	struct pa_ss_info {
		std::string name = "";
		int volume = 0;
		bool is_muted = false;
		pa_cvolume pa_volume;
		uint8_t pa_channels;
	} pa_def_sink_info, pa_def_source_info;

	std::vector<std::function<void(float)>> m_input_volume_changed_callbacks;
	std::vector<std::function<void(float)>> m_output_volume_changed_callbacks;

	std::vector<std::function<void(bool)>> m_input_mute_changed_callbacks;
	std::vector<std::function<void(bool)>> m_output_mute_changed_callbacks;

public:
	pulseaudio();
	~pulseaudio();

	virtual auto initialize() -> bool;
	virtual auto destroy() -> void;

	virtual auto run() -> int;
	auto quit_main_loop(int ret) -> void;

	virtual auto get_input_volume_in_percent() -> float;
	virtual auto set_input_volume_in_percent(float volume) -> void;
	virtual auto mute_input() -> void;
	virtual auto unmute_input() -> void;

	virtual auto get_output_volume_in_percent() -> float;
	virtual auto set_output_volume_in_percent(float volume) -> void;
	virtual auto mute_output() -> void;
	virtual auto unmute_output() -> void;

	virtual auto callback_input_volume_changed(std::function<void(float)> callback) -> void;
	virtual auto callback_input_mute_changed(std::function<void(bool)> callback) -> void;

	virtual auto callback_output_volume_changed(std::function<void(float)> callback) -> void;
	virtual auto callback_output_mute_changed(std::function<void(bool)> callback) -> void;

	auto pa_context_state_change_callback() -> void;
	auto pa_context_server_info_callback(const pa_server_info *info) -> void;

	auto pa_context_sink_info_callback(const pa_sink_info *info, int eol) -> void;
	auto pa_context_source_info_callback(const pa_source_info *info, int eol) -> void;

	auto pa_context_subscribe_callback(pa_subscription_event_type_t type, uint32_t idx) -> void;

private:
	static auto redirect_context_state_change_callback(pa_context *ctx, void *userdata) -> void {
		assert(ctx && userdata);

		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_state_change_callback();
	}

	static auto redirect_context_server_info_callback(pa_context *ctx, const pa_server_info *info, void *userdata)
		-> void {
		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_server_info_callback(info);
	}

	static auto redirect_context_sink_info_callback(pa_context *ctx, const pa_sink_info *info, int eol, void *userdata)
		-> void {
		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_sink_info_callback(info, eol);
	}

	static auto redirect_context_source_info_callback(pa_context *ctx, const pa_source_info *info, int eol,
													  void *userdata) -> void {
		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_source_info_callback(info, eol);
	}

	static auto redirect_context_subscribe_callback(pa_context *ctx, pa_subscription_event_type_t type, uint32_t idx,
													void *userdata) -> void {
		pulseaudio *pa_backend = (pulseaudio *)userdata;
		pa_backend->pa_context_subscribe_callback(type, idx);
	}
};

} // namespace wapanel::applet::backends
