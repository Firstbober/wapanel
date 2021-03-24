#pragma once

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

namespace wapanel::applet::utils {

auto double_fork() -> int {
	pid_t fork_id;
	if ((fork_id = fork()) == 0) {
		setsid();

		signal(SIGCHLD, SIG_IGN);
		signal(SIGHUP, SIG_IGN);

		if (fork() == 0) {
			umask(0);

			int x;
			for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
				close(x);
			}

			return 0;
		} else {
			exit(0);
		}
	} else {
		waitpid(fork_id, 0, 0);
		return 1;
	}
}

};