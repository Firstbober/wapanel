#pragma once

#include <string>
#include <wlr-foreign-toplevel-management-unstable-v1-client-protocol.h>

namespace wapanel::applet::wl {

enum class toplevel_state { ACTIVATED = 0, MAXIMIZED = 1, MINIMIZED = 2 };

struct toplevel {
	std::string title;
	std::string app_id;

	toplevel(zwlr_foreign_toplevel_handle_v1 *handle);
	~toplevel();

private:
	zwlr_foreign_toplevel_handle_v1 *m_handle;
};

}