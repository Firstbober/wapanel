#pragma once
#include "../backend.hh"

namespace wapanel::applet::backends {

class pulseaudio : public backend {
public:
	pulseaudio();
	~pulseaudio();

	virtual int get_input_volume_in_percent();
	virtual void set_input_volume_in_percent();
	virtual void mute_input();
	virtual void unmute_input();

	virtual int get_output_volume_in_percent();
	virtual void set_output_volume_in_percent();
	virtual void mute_output();
	virtual void unmute_output();
};

} // namespace wapanel::applet::backend
