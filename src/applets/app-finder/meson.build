library('wapanel-applet-app-finder',
	[
		'main.cc',
		'app-finder.cc',
		'menu_entries.cc'
	],
	dependencies: [
		dependency('gtk+-3.0'),
		dependency('pugixml')
	],
	install: true,
	include_directories: [
		include_directories('../../lib'),
		include_directories('../../../thirdparty')
	],
	install_dir: 'lib/wapanel/applets',
	link_with: [appletapi_lib, icon_cache_util]
)