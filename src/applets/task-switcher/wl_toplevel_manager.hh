#pragma once

#include <wlr-foreign-toplevel-management-unstable-v1-client-protocol.h>

namespace wapanel::applet::wl {

class toplevel_manager {
private:
	toplevel_manager() {};
	toplevel_manager(const toplevel_manager &) {};

public:
	static toplevel_manager &get() {
		static toplevel_manager singleton;
		return singleton;
	}
};

}