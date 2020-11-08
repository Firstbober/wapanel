#pragma once
#include "../backend.hh"

namespace wapanel::applet::backends {

class pulseaudio : public backend {
public:
	pulseaudio();
	~pulseaudio();

	virtual auto get_input_volume_in_percent() -> int;
	virtual auto set_input_volume_in_percent() -> void;
	virtual auto mute_input() -> void;
	virtual auto unmute_input() -> void;

	virtual auto get_output_volume_in_percent() -> int;
	virtual auto set_output_volume_in_percent() -> void;
	virtual auto mute_output() -> void;
	virtual auto unmute_output() -> void;
};

} // namespace wapanel::applet::backend
