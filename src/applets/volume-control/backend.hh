#pragma once
#include <functional>

namespace wapanel::applet {

class backend
{
public:
	// Initializes backend
	virtual auto initialize() -> bool = 0;
	// Destroys backend, class destructor should call
	virtual auto destroy() -> void = 0;

	virtual auto run() -> int = 0;

	virtual auto get_input_volume_in_percent() -> float = 0;
	virtual auto set_input_volume_in_percent() -> void = 0;
	virtual auto mute_input() -> void = 0;
	virtual auto unmute_input() -> void = 0;

	virtual auto get_output_volume_in_percent() -> float = 0;
	virtual auto set_output_volume_in_percent() -> void = 0;
	virtual auto mute_output() -> void = 0;
	virtual auto unmute_output() -> void = 0;

	virtual auto callback_input_volume_changed(std::function<void(float)> callback) -> void = 0;
	virtual auto callback_input_mute_changed(std::function<void(bool)> callback) -> void = 0;

	virtual auto callback_output_volume_changed(std::function<void(float)> callback) -> void = 0;
	virtual auto callback_output_mute_changed(std::function<void(bool)> callback) -> void = 0;

	virtual ~backend() = default;
};


} // namespace wapanel::applet
