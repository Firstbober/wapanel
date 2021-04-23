library('wapanel-applet-volume-control',
	[
		'main.cc',
		'volume-control.cc',

		'backends/pulseaudio.cc'
	],
	dependencies: [
		gtk3,
		dependency('libpulse'),
		dependency('threads')
	],
	install: true,
	include_directories: include_directories('../../lib'),
	install_dir: applet_install_dir,
	link_with: [appletapi_lib, icon_cache_util]
)