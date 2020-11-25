icon_cache_util = static_library(
	'icon-cache-util',
	['icon_cache.cc'],
	dependencies: dependency('gtk+-3.0'),
	include_directories: '../../thirdparty'
)

if get_option('activator')
	subdir('activator')
endif

if get_option('app-finder')
	subdir('app-finder')
endif

if get_option('clock')
	subdir('clock')
endif

if get_option('separator')
	subdir('separator')
endif

if get_option('task-switcher')
	subdir('task-switcher')
endif

if get_option('volume-control')
	subdir('volume-control')
endif