#include "pulseaudio.hh"
#include "../../../log.hh"
#include <signal.h>

namespace wapanel::applet::backends {

pulseaudio::pulseaudio()
	: m_mainloop(NULL)
	, m_mainloop_api(NULL)
	, m_context(NULL) {}
pulseaudio::~pulseaudio() { this->destroy(); }

auto pulseaudio::initialize() -> bool {
	m_mainloop = pa_mainloop_new();
	if (!m_mainloop) {
		log_error("Failed to create PulseAudio main loop");
		return false;
	}

	m_mainloop_api = pa_mainloop_get_api(m_mainloop);
	if (pa_signal_init(m_mainloop_api) != 0) {
		log_error("Failed to init and bind UNIX signal subsystem to PulseAudio main loop");
		return false;
	}

	m_context = pa_context_new(m_mainloop_api, "Wapanel - volume control applet");
	if (!m_context) {
		log_error("Failed to create PulseAudio context");
		return false;
	}

	if (pa_context_connect(m_context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL) < 0) {
		log_error("Failed to connect context with PulseAudio server, details: %s",
				  pa_strerror(pa_context_errno(m_context)));
		return false;
	}

	pa_context_set_state_callback(m_context, this->redirect_context_state_change_callback, this);

	log_info("Successfully initialized PulseAudio backend");

	return true;
}

auto pulseaudio::destroy() -> void {
	this->quit_main_loop(0);

	if (m_context) {
		pa_context_unref(m_context);
		m_context = NULL;
	}

	if (m_mainloop) {
		pa_mainloop_free(m_mainloop);
		m_mainloop = NULL;
		m_mainloop_api = NULL;
	}


	log_info("Destroyed PulseAudio backend");
}

auto pulseaudio::run() -> int {
	int ret = 1;

	if (pa_mainloop_run(m_mainloop, &ret) < 0) {
		log_error("PulseAudio mainloop execution failed");
		return ret;
	}

	return ret;
}

auto pulseaudio::quit_main_loop(int ret) -> void { pa_mainloop_quit(m_mainloop, ret); }

auto pulseaudio::get_input_volume_in_percent() -> int {}
auto pulseaudio::set_input_volume_in_percent() -> void {}
auto pulseaudio::mute_input() -> void {}
auto pulseaudio::unmute_input() -> void {}

auto pulseaudio::get_output_volume_in_percent() -> int {}
auto pulseaudio::set_output_volume_in_percent() -> void {}
auto pulseaudio::mute_output() -> void {}
auto pulseaudio::unmute_output() -> void {}

auto pulseaudio::pa_context_state_change_callback(pa_context *ctx) -> void {
	switch (pa_context_get_state(ctx)) {
	case PA_CONTEXT_CONNECTING:
	case PA_CONTEXT_AUTHORIZING:
	case PA_CONTEXT_SETTING_NAME:
		break;

	case PA_CONTEXT_READY:
		log_info("PulseAudio connection established");
		pa_context_get_server_info(ctx, this->redirect_context_server_info_callback, this);

		break;

	case PA_CONTEXT_TERMINATED:
		log_error("PulseAudio connection terminated");
		this->quit_main_loop(0);
		break;

	case PA_CONTEXT_FAILED:
	default:
		log_error("PulseAudio connection failed: %s", pa_strerror(pa_context_errno(ctx)));
		this->quit_main_loop(1);
		break;
	}
}

auto pulseaudio::pa_context_server_info_callback(pa_context *ctx, const pa_server_info *info) -> void {
	log_warn("server info");
	// TODO: Get default sink and source
}
auto pulseaudio::pa_context_sink_info_callback(pa_context *ctx, const pa_sink_info *info, int eol) -> void {
	log_warn("sink info");
	// TODO: Implement everything that might be needed here.
}

} // namespace wapanel::applet::backends
