#include "app-finder.hh"

namespace wapanel::applet {

namespace ui_comps {

	logout_box::logout_box() {}
	logout_box::~logout_box() {}

	action_bar::action_bar() {}
	action_bar::~action_bar() {}

	list_area::list_area() {}
	list_area::~list_area() {}

};

app_finder::app_finder(wap_t_applet_config applet_config, int id)
	: m_id(id) {}
app_finder::~app_finder() {}

auto app_finder::get_widget() -> GtkWidget * {
	return gtk_label_new(" || app-finder || "); // TODO: Insert real widget here
}

} // namespace wapanel::applet
