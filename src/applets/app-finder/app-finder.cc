#include "app-finder.hh"

namespace wapanel::applet {

app_finder::app_finder(wap_t_applet_config applet_config, int id)
	: m_id(id) {}
app_finder::~app_finder() {}

auto app_finder::get_widget() -> GtkWidget * {
	return gtk_label_new(" || app-finder || "); // TODO: Insert real widget here
}

} // namespace wapanel::applet
