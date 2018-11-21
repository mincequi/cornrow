#include "daemon.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

void daemonize()
{
    pid_t pid = fork();
    if (pid < 0) {
        printf("fork error");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    if (setsid() < 0) {
        printf("setsid error");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        printf("chdir error");
        exit(EXIT_FAILURE);
    }

    // close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
