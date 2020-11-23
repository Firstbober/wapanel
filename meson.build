project('wapanel', 'cpp',
	version: '0.1.0',
	default_options: [
		'cpp_std=c++2a',
		'build.cpp_std=c++2a'
	])
add_languages('c', required: true, native: false)

gtklayershell  = dependency('gtk-layer-shell-0', version: '>= 0.1', fallback: ['gtk-layer-shell', 'gtk_layer_shell_dep'])
dynlink        = meson.get_compiler('cpp').find_library('dl', required: true)
pkgconfig      = import('pkgconfig')

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
	'src/wap_t_convert.cc',
	'src/utils.cc'
]

appletapi_lib = static_library('wapanel-appletapi', ['src/lib/appletapi.c'], install: true)
install_headers('src/lib/appletapi.h', subdir: 'wapanel-appletapi')
pkgconfig.generate(appletapi_lib)

subdir('src/applets')

install_data('3TH_PARTY_LICENSES', install_dir : get_option('datadir') / 'wapanel')
install_data('wapanel.toml', install_dir : get_option('datadir') / 'wapanel')
install_data('wapanel.css', install_dir : get_option('datadir') / 'wapanel')

conf_data = configuration_data()
conf_data.set('version', meson.project_version())

if get_option('buildtype').startswith('release')
  conf_data.set('BUILD_RELEASE', true)
endif

conf_data.set('data_dir', get_option('datadir'))

configure_file(input : 'config.hh.in',
               output : 'config.hh',
               configuration : conf_data)

executable(
	'wapanel',
	sources,
	dependencies: deps,
	install: true,
	include_directories : include_directories('thirdparty')
)