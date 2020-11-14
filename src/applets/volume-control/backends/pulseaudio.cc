#include "pulseaudio.hh"
#include "../../../log.hh"
#include <signal.h>

namespace wapanel::applet::backends {

pulseaudio::pulseaudio()
	: m_mainloop(NULL)
	, m_mainloop_api(NULL)
	, m_context(NULL)
	, m_signal(NULL) {}
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

	m_signal = pa_signal_new(SIGINT, this->exit_signal_callback, this);
	if (!m_signal) {
		log_error("Failed to create UNIX signal event source object");
		return false;
	}
	signal(SIGPIPE, SIG_IGN);

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

	pa_context_set_state_callback(m_context, this->ex_context_state_change_callback, this);

	log_info("Successfully initialized PulseAudio backend");

	return true;
}

auto pulseaudio::destroy() -> void {
	if (m_context) {
		pa_context_unref(m_context);
		m_context = NULL;
	}

	if (m_signal) {
		pa_signal_free(m_signal);
		pa_signal_done();
		m_signal = NULL;
	}

	if (m_mainloop) {
		pa_mainloop_free(m_mainloop);
		m_mainloop = NULL;
		m_mainloop_api = NULL;
	}

	log_info("Destroyed PulseAudio backend");
}

auto pulseaudio::get_input_volume_in_percent() -> int {}
auto pulseaudio::set_input_volume_in_percent() -> void {}
auto pulseaudio::mute_input() -> void {}
auto pulseaudio::unmute_input() -> void {}

auto pulseaudio::get_output_volume_in_percent() -> int {}
auto pulseaudio::set_output_volume_in_percent() -> void {}
auto pulseaudio::mute_output() -> void {}
auto pulseaudio::unmute_output() -> void {}

auto pulseaudio::pa_context_state_change_callback(pa_context *ctx) -> void {}

} // namespace wapanel::applet::backends
