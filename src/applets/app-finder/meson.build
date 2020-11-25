library('wapanel-applet-app-finder',
	[
		'main.cc',
		'app-finder.cc'
	],
	dependencies: [
		dependency('gtk+-3.0')
	],
	install: true,
	include_directories: include_directories('../../lib'),
	install_dir: 'lib/wapanel/applets',
	link_with: [appletapi_lib, icon_cache_util]
)