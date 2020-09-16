project('wapanel', 'cpp',
	version: '0.1.0',
	default_options: [
		'cpp_std=c++2a',
		'build.cpp_std=c++2a'
	])
add_languages('c', required: true, native: false)

gtklayershell  = dependency('gtk-layer-shell-0', version: '>= 0.1', fallback: ['gtk-layer-shell', 'gtk_layer_shell_dep'])
dynlink        = meson.get_compiler('cpp').find_library('dl', required: true)

deps = [
	dependency('gtk+-3.0'),
	dependency('gio-2.0'),

	gtklayershell,
	dynlink
]

sources = [
	'src/main.cc',
	'src/panel.cc',
	'src/config.cc',
	'src/applets.cc',
	'src/wap_t_convert.cc'
]

subdir('src/applets')

executable(
	'wapanel',
	sources,
	dependencies: deps,
	install: true,
	include_directories : include_directories('thirdparty')
)
install_headers('src/lib/appletapi.h', subdir: 'wapanel')