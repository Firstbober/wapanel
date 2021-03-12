#include "logout_box.hh"
#include "../../icon_cache.hh"
#include <functional>

namespace ui_comps {

logout_box::logout_box(config conf)
	: m_root(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 4)))
	, cmp_config(conf) {

	std::array<std::reference_wrapper<std::string>, 6> vars_to_check
		= { cmp_config.shutdown_cmd, cmp_config.restart_cmd,   cmp_config.logout_cmd,
			cmp_config.suspend_cmd,	 cmp_config.hibernate_cmd, cmp_config.lock_cmd };
	std::array<std::string, 6> labels = { "Shutdown", "Restart", "Logout", "Suspend", "Hibernate", "Lock" };
	std::array<std::string, 6> icons
		= { "system-shutdown-symbolic", "system-reboot-symbolic", "system-log-out-symbolic",
			"night-light-symbolic",		"process-stop-symbolic",  "system-lock-screen-symbolic" };

	for (size_t i = 0; i < vars_to_check.size(); i++) {
		if (vars_to_check[i].get() != "") {
			m_action_buttons[i] = GTK_BUTTON(gtk_button_new());
			m_action_btn_container[i] = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));

			gtk_button_set_relief(m_action_buttons[i], GTK_RELIEF_NONE);

			gtk_box_pack_start(
				m_action_btn_container[i],
				GTK_WIDGET(gtk_image_new_from_pixbuf(wapanel::applet::utils::ic::get_icon(icons[i], 20))), false, true,
				0);

			gtk_box_pack_start(m_action_btn_container[i], GTK_WIDGET(gtk_label_new(labels[i].c_str())), false, true, 0);

			gtk_container_add(GTK_CONTAINER(m_action_buttons[i]), GTK_WIDGET(m_action_btn_container[i]));
			gtk_box_pack_start(m_root, GTK_WIDGET(m_action_buttons[i]), false, true, 0);

			g_signal_connect(m_action_buttons[i], "clicked", G_CALLBACK(+[](GtkButton *btn, std::string *cmd) {
								 g_spawn_command_line_async(std::string("sh -c \"" + *cmd + "\"").c_str(), NULL);
							 }),
							 &vars_to_check[i].get());
		}
	}

	log_info("app-finder/logout-box created");
}
logout_box::~logout_box() {}
auto logout_box::get_widget() -> GtkWidget * { return GTK_WIDGET(m_root); }
};