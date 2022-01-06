library('wapanel-applet-app-finder',
	[
		'main.cc',
		'app-finder.cc',

		'components/action_bar.cc',
		'components/list_area.cc',
		'components/logout_box.cc',

		'search-engine/search_engine.cc',
		'search-engine/search_engine_globals.cc'
	],
	dependencies: [
		gtk3,
		gio2unix,
		dependency('threads'),
		gtklayershell
	],
	install: true,
	include_directories: [
		include_directories('../../lib'),
		wapanel_include_directories
	],
	install_dir: applet_install_dir,
	link_with: [appletapi_lib, icon_cache_util]
)