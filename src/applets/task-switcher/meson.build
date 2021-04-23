# Used code from https://github.com/WayfireWM/wf-shell/blob/master/meson.build

wayland_protos = dependency('wayland-protocols')
wayland_client = dependency('wayland-client')

wayland_scanner = find_program('wayland-scanner')

wayland_scanner_code = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.c',
	arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_client = generator(
	wayland_scanner,
	output: '@BASENAME@-client-protocol.h',
	arguments: ['client-header', '@INPUT@', '@OUTPUT@'],
)

client_protocols = [
    'wlr-foreign-toplevel-management-unstable-v1.xml'
]

wl_protos_src = []
wl_protos_headers = []

foreach p : client_protocols
	xml = join_paths(p)
	wl_protos_headers += wayland_scanner_client.process(xml)
	wl_protos_src += wayland_scanner_code.process(xml)
endforeach

lib_wl_protos = static_library('wl_protos', wl_protos_src + wl_protos_headers,
	dependencies: [wayland_client]) # for the include directory

wl_protos = declare_dependency(
	link_with: lib_wl_protos,
	sources: wl_protos_headers,
)

# Used code from https://github.com/WayfireWM/wf-shell/blob/master/meson.build

library('wapanel-applet-task-switcher',
	[
		'main.cc',

		'wl_toplevel_manager.cc',
		'wl_toplevel.cc',

		'task_switcher.cc'
	],
	dependencies: [
		gtk3,
		gio2,
		wl_protos
	],
	install: true,
	include_directories: include_directories('../../lib'),
	install_dir: applet_install_dir,
	link_with: appletapi_lib
)