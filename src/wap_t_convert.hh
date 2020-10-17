#pragma once

#include "lib/appletapi.h"
#include <vector>
#include <toml11/toml.hpp>

namespace wapanel::conv
{

// Convert exisitng applet config in toml to
// handy-dandy c-formaty struct.
auto convert_toml_to_wap_t_config_variable(toml::value& value, unsigned int panel_height = 0) -> _wap_t_config_variable*;

// Remove content from handy-dandy c-formaty struct
// to prevent memory leaks.
auto free_wap_t_config_variable(_wap_t_config_variable variable) -> void;

}
