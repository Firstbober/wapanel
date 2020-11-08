#include "pulseaudio.hh"

namespace wapanel::applet::backends {

pulseaudio::pulseaudio() {}
pulseaudio::~pulseaudio() {}

auto pulseaudio::get_input_volume_in_percent() -> int {}
auto pulseaudio::set_input_volume_in_percent() -> void {}
auto pulseaudio::mute_input() -> void {}
auto pulseaudio::unmute_input() -> void {}

auto pulseaudio::get_output_volume_in_percent() -> int {}
auto pulseaudio::set_output_volume_in_percent() -> void {}
auto pulseaudio::mute_output() -> void {}
auto pulseaudio::unmute_output() -> void {}

} // namespace wapanel::applet::backend
