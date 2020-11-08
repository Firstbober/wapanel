#pragma once

namespace wapanel::applet {

class backend
{
public:
	virtual int get_input_volume_in_percent() = 0;
	virtual void set_input_volume_in_percent() = 0;
	virtual void mute_input() = 0;
	virtual void unmute_input() = 0;

	virtual int get_output_volume_in_percent() = 0;
	virtual void set_output_volume_in_percent() = 0;
	virtual void mute_output() = 0;
	virtual void unmute_output() = 0;

	virtual ~backend() = default;
};


} // namespace wapanel::applet
