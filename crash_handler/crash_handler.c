#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../../testing.h/testing.h"

#define NO_START_VAL 180
#define DEFAULT_EXEC "./SquireDesktop"
#define LOG_FILE "squiredesktop.log"

int main(int argc, char **argv)
{
    char *exec_file = DEFAULT_EXEC;
    if (argc == 2) {
        exec_file = argv[1];
    } else if (argc > 2) {
        lprintf(LOG_ERROR, "Usage %s <squire desktop file_ame>\n", argv[0]);
        return 1;
    }

    lprintf(LOG_INFO, "Starting %s\n", exec_file);

    int fid[2]; // r, w for stderr
    ASSERT(pipe(fid) == 0);

    int pid = fork();
    if (pid == 0) {
        // Close read end
        close(fid[0]);

        // Pipe stderr and, stdout to the pipe.
        dup2(fid[1], STDERR_FILENO);
        dup2(fid[1], STDOUT_FILENO);

        execlp(exec_file, exec_file, "--crash-handler");

        // If the process has not been replaced then exec failed.
        lprintf(LOG_ERROR, "Cannot start %s. Errno %d (%s)\n", exec_file, errno, strerror(errno));

        close(fid[1]);
        return NO_START_VAL;
    } else {
        // Close write end
        close(fid[1]);

        FILE *pipe_r = fdopen(fid[0], "rb");
        ASSERT(pipe_r != NULL);

        FILE *log_file = fopen(LOG_FILE, "w");
        if (log_file == NULL) {
            lprintf(LOG_ERROR, "Cannot log the application.\n");
            return 1;
        }

        for (int c; c = fgetc(pipe_r), c != EOF;) {
            fputc(c, stdout);
            fflush(stdout);

            fputc(c, log_file);
            fflush(log_file);
        }
        close(fid[0]);

        // Close pipe and, log
        fclose(log_file);
        fclose(pipe_r);

        int r = -1;
        int pid2 = waitpid(pid, &r, 0);

        // Check that the right process was waited for.
        ASSERT(pid == pid2);

        // If the program did not exit with code then handle errors
        // code 9 (sigkll) is also ignored.
        if (r == NO_START_VAL) {
            return 1;
        } else if (r != 0 && r != 9) {
            lprintf(LOG_ERROR, "Crash detected in squire desktop\n");
            return 1;
        }
    }

    return 0;
}
