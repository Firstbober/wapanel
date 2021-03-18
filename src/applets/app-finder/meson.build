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
		dependency('gtk+-3.0'),
		dependency('threads'),
		gtklayershell
	],
	install: true,
	include_directories: [
		include_directories('../../lib'),
		include_directories('../../../thirdparty')
	],
	install_dir: 'lib/wapanel/applets',
	link_with: [appletapi_lib, icon_cache_util]
)