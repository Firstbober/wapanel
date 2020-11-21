library('wapanel-applet-volume-control',
	[
		'main.cc',
		'volume-control.cc',
		'icon-cache.cc',

		'backends/pulseaudio.cc'
	],
	dependencies: [
		dependency('gtk+-3.0'),
		dependency('libpulse'),
		dependency('threads')
	],
	install: true,
	include_directories: include_directories('../../lib'),
	install_dir: 'lib/wapanel/applets',
	link_with: appletapi_lib
)