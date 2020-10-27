#pragma once
#include <stdio.h>

#define _LOG_COLOR_RED	  "\033[31m"
#define _LOG_COLOR_YELLOW "\033[33m"
#define _LOG_COLOR_BLUE	  "\033[34m"
#define _LOG_COLOR_RESET  "\033[0m"

#define log_error(log, ...)                                                                                            \
	fprintf(stderr, _LOG_COLOR_RED "[wapanel] [PERR] (" __FILE__ ":%i): " log "\n" _LOG_COLOR_RESET, __LINE__,         \
			##__VA_ARGS__)

#define log_warn(log, ...)                                                                                             \
	fprintf(stderr, _LOG_COLOR_YELLOW "[wapanel] [WARN] (" __FILE__ ":%i): " log "\n" _LOG_COLOR_RESET, __LINE__,      \
			##__VA_ARGS__)

#define log_info(log, ...)                                                                                             \
	fprintf(stderr, _LOG_COLOR_BLUE "[wapanel] [INFO] (" __FILE__ ":%i): " log "\n" _LOG_COLOR_RESET, __LINE__,        \
			##__VA_ARGS__)