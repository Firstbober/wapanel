#pragma once
#include "backend.hh"
#include <appletapi.h>
#include <atomic>
#include <gtk/gtk.h>
#include <string>

/*
	Default config
	-1 stands for default value

	[[panel.applet]]
		name = "volume-control"
		icon_height = -1
		flat = false
		sound_mixer = ""
*/

namespace wapanel::applet {

class volume_widget {
private:
	backend *m_backend;

	GtkBox *m_root;
	GtkImage *m_ind_icon;
	GtkAdjustment *m_volume_adj;
	GtkScale *m_volume_scale;
	GtkSwitch *m_mute_switch;

	std::atomic_bool is_icon_change_locked;
	std::atomic_bool is_switched_outside;

	enum class vol_state { muted, low, medium, high };
	std::atomic<vol_state> volume_state;

	struct _state_set_data {
		std::atomic_bool *v1;
		backend *v2;
	};
	_state_set_data *state_set_data = NULL;

public:
	volume_widget(backend *backend, bool type_volume);
	~volume_widget();

	auto get_widget() -> GtkWidget *;
};

class volume_control {
private:
	backend *m_backend;
	GtkMenuButton *m_pop_control;
	GtkImage *m_button_icon;
	GtkPopover *m_control_container;
	GtkBox *m_vol_widget_list;
	volume_widget *m_output_widget, *m_input_widget;
	GtkButton *m_open_sound_mixer;

	struct config {
		unsigned int __panel_height;

		int icon_height = -1;
		bool is_flat = false;

		std::string sound_mixer_executable;
	} m_config;

public:
	volume_control(wap_t_applet_config applet_config, backend *backend);
	~volume_control();

	auto get_widget() -> GtkWidget *;
};

} // namespace wapanel::applet::volume_control
