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

	this->m_input_volume_changed_callbacks.clear();
	this->m_output_volume_changed_callbacks.clear();

	this->m_input_mute_changed_callbacks.clear();
	this->m_output_mute_changed_callbacks.clear();

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

auto pulseaudio::get_input_volume_in_percent() -> float {
	return ((float)this->pa_def_source_info.volume / (float)PA_VOLUME_NORM) * 100.f;
}
auto pulseaudio::set_input_volume_in_percent(float volume) -> void {
	pa_cvolume_set(&(this->pa_def_source_info.pa_volume), this->pa_def_source_info.pa_channels,
				   static_cast<int>((volume * PA_VOLUME_NORM) / 100.f));
	pa_context_set_source_volume_by_name(this->m_context, this->pa_def_source_info.name.c_str(),
										 &(this->pa_def_source_info.pa_volume), NULL, NULL);
}
auto pulseaudio::is_input_muted() -> bool {
	return this->pa_def_source_info.is_muted;
}
auto pulseaudio::mute_input() -> void {
	log_info("Muted PulseAudio input");
	pa_context_set_source_mute_by_name(this->m_context, this->pa_def_source_info.name.c_str(), true, NULL, NULL);
}
auto pulseaudio::unmute_input() -> void {
	log_info("Unmuted PulseAudio input");
	pa_context_set_source_mute_by_name(this->m_context, this->pa_def_source_info.name.c_str(), false, NULL, NULL);
}


auto pulseaudio::get_output_volume_in_percent() -> float {
	return ((float)this->pa_def_sink_info.volume / (float)PA_VOLUME_NORM) * 100.f;
}
auto pulseaudio::set_output_volume_in_percent(float volume) -> void {
	pa_cvolume_set(&(this->pa_def_sink_info.pa_volume), this->pa_def_sink_info.pa_channels,
				   static_cast<int>((volume * PA_VOLUME_NORM) / 100.f));
	pa_context_set_sink_volume_by_name(this->m_context, this->pa_def_sink_info.name.c_str(),
									   &(this->pa_def_sink_info.pa_volume), NULL, NULL);
}
auto pulseaudio::is_output_muted() -> bool {
	return this->pa_def_sink_info.is_muted;
}
auto pulseaudio::mute_output() -> void {
	log_info("Muted PulseAudio output");
	pa_context_set_sink_mute_by_name(this->m_context, this->pa_def_sink_info.name.c_str(), true, NULL, NULL);
}
auto pulseaudio::unmute_output() -> void {
	log_info("Unmuted PulseAudio output");
	pa_context_set_sink_mute_by_name(this->m_context, this->pa_def_sink_info.name.c_str(), false, NULL, NULL);
}

auto pulseaudio::callback_input_volume_changed(std::function<void(float)> callback) -> void {
	m_input_volume_changed_callbacks.push_back(callback);
}
auto pulseaudio::callback_input_mute_changed(std::function<void(bool)> callback) -> void {
	m_input_mute_changed_callbacks.push_back(callback);
}

auto pulseaudio::callback_output_volume_changed(std::function<void(float)> callback) -> void {
	m_output_volume_changed_callbacks.push_back(callback);
}
auto pulseaudio::callback_output_mute_changed(std::function<void(bool)> callback) -> void {
	m_output_mute_changed_callbacks.push_back(callback);
}

auto pulseaudio::pa_context_state_change_callback() -> void {
	switch (pa_context_get_state(m_context)) {
	case PA_CONTEXT_CONNECTING:
	case PA_CONTEXT_AUTHORIZING:
	case PA_CONTEXT_SETTING_NAME:
		break;

	case PA_CONTEXT_READY:
		log_info("PulseAudio connection established");
		pa_context_get_server_info(m_context, this->redirect_context_server_info_callback, this);

		pa_context_set_subscribe_callback(m_context, this->redirect_context_subscribe_callback, this);
		pa_context_subscribe(m_context, PA_SUBSCRIPTION_MASK_ALL, NULL, NULL);
		break;

	case PA_CONTEXT_TERMINATED:
		log_error("PulseAudio connection terminated");
		this->quit_main_loop(0);
		break;

	case PA_CONTEXT_FAILED:
	default:
		log_error("PulseAudio connection failed: %s", pa_strerror(pa_context_errno(m_context)));
		this->quit_main_loop(1);
		break;
	}
}

auto pulseaudio::pa_context_server_info_callback(const pa_server_info *info) -> void {
	log_info("Default PulseAudio sink name: %s", info->default_sink_name);
	log_info("Default PulseAudio source name: %s", info->default_source_name);

	pa_context_get_sink_info_by_name(m_context, info->default_sink_name, this->redirect_context_sink_info_callback,
									 this);
	pa_context_get_source_info_by_name(m_context, info->default_source_name,
									   this->redirect_context_source_info_callback, this);
}

auto pulseaudio::pa_context_sink_info_callback(const pa_sink_info *info, int eol) -> void {
	if (info) {
		int old_volume = this->pa_def_sink_info.volume;
		bool old_mute_value = this->pa_def_sink_info.is_muted;

		this->pa_def_sink_info.name = std::string(info->name);
		this->pa_def_sink_info.volume = pa_cvolume_avg(&(info->volume));
		this->pa_def_sink_info.is_muted = info->mute;
		this->pa_def_sink_info.pa_volume = info->volume;
		this->pa_def_sink_info.pa_channels = info->channel_map.channels;

		if (old_volume != this->pa_def_sink_info.volume) {
			for (auto &&callback : this->m_output_volume_changed_callbacks) {
				callback(this->get_output_volume_in_percent());
			}
		}

		if (old_mute_value != this->pa_def_sink_info.is_muted) {
			for (auto &&callback : this->m_output_mute_changed_callbacks) {
				callback(this->pa_def_sink_info.is_muted);
			}
		}
	}
}

auto pulseaudio::pa_context_source_info_callback(const pa_source_info *info, int eol) -> void {
	if (info) {
		int old_volume = this->pa_def_source_info.volume;
		bool old_mute_value = this->pa_def_source_info.is_muted;

		this->pa_def_source_info.name = std::string(info->name);
		this->pa_def_source_info.volume = pa_cvolume_avg(&(info->volume));
		this->pa_def_source_info.is_muted = info->mute;
		this->pa_def_source_info.pa_volume = info->volume;
		this->pa_def_source_info.pa_channels = info->channel_map.channels;

		if (old_volume != this->pa_def_source_info.volume) {
			for (auto &&callback : this->m_input_volume_changed_callbacks) {
				callback(this->get_input_volume_in_percent());
			}
		}

		if (old_mute_value != this->pa_def_source_info.is_muted) {
			for (auto &&callback : this->m_input_mute_changed_callbacks) {
				callback(this->pa_def_source_info.is_muted);
			}
		}
	}
}

auto pulseaudio::pa_context_subscribe_callback(pa_subscription_event_type_t type, uint32_t idx) -> void {
	unsigned facility = type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
	pa_operation *op = NULL;

	switch (facility) {
	case PA_SUBSCRIPTION_EVENT_SINK:
		pa_context_get_sink_info_by_index(m_context, idx, this->redirect_context_sink_info_callback, this);
		break;

	case PA_SUBSCRIPTION_EVENT_SOURCE:
		pa_context_get_source_info_by_index(m_context, idx, this->redirect_context_source_info_callback, this);
		break;

	default:
		break;
	}

	if (op) pa_operation_unref(op);
}

} // namespace wapanel::applet::backends
