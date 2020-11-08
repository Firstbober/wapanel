#include "pulseaudio.hh"

namespace wapanel::applet::backends {

pulseaudio::pulseaudio() {}
pulseaudio::~pulseaudio() {}

int pulseaudio::get_input_volume_in_percent() {}
void pulseaudio::set_input_volume_in_percent() {}
void pulseaudio::mute_input() {}
void pulseaudio::unmute_input() {}

int pulseaudio::get_output_volume_in_percent() {}
void pulseaudio::set_output_volume_in_percent() {}
void pulseaudio::mute_output() {}
void pulseaudio::unmute_output() {}

} // namespace wapanel::applet::backend
