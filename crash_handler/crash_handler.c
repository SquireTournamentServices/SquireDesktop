#include <stdio.h>
#include <stdlib.h>
#ifdef WINDOWS
#include <io.h>
#include <process.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include "./config_reader.h"
#include "./webhooks.h"
#include "./escape_seq.h"
#include "./path_change.h"
#include "../testing_h/testing.h"

#define NO_START_VAL 180
#define DEFAULT_EXEC "SquireDesktop"
#define LOG_FILE "squiredesktop.log"
#define MSG_START "```py\n"
#define MSG_END "\n```"
#define DISCORD_WEBHOOK_URL "https://discord.com/api/webhooks/1039929537477222521/EZoDsiu0_-zlSwFmdT6S9M5GMznfJkQR86kjRObIy5ccnCQhNCAZd3pAvUS31AtOBMDW"
#define DISCORD_MESSAGE_LIMIT (2000 - sizeof(MSG_START) - 1 - sizeof(MSG_END) - 1)
#define FAILED_START_MSG "Failed to start"

static char in_memory_log[DISCORD_MESSAGE_LIMIT];
static size_t f_ptr, b_ptr;

int main(int argc, char **argv)
{
    if (!change_path()) {
        lprintf(LOG_ERROR, "Cannot get to the Squire Desktop data folder.\n");
    }

    char *exec_file = DEFAULT_EXEC;
    if (argc == 2) {
        exec_file = argv[1];
    } else if (argc > 2) {
        lprintf(LOG_ERROR, "Usage %s <squire desktop file_name>\n", argv[0]);
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
        execl(exec_file, exec_file, "--crash-handler");

        // If the process has not been replaced then exec failed.
        lprintf(LOG_ERROR, "Cannot start %s. Errno %d (%s)\n", exec_file, errno, strerror(errno));

        close(fid[1]);
        return NO_START_VAL;
    } else {
        // Close write end
        close(fid[1]);

        f_ptr = b_ptr = 0;

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

            // Change status
            if (c == ANSI_RED[0] || c == ANSI_RESET[0]) {
                read_ansi_sequence(pipe_r, log_file, c);
            } else if (c != 0 && c != EOF) {
                in_memory_log[b_ptr] = c;
                b_ptr++;
                b_ptr %= sizeof(in_memory_log);

                // Truncate the log.
                if (b_ptr == f_ptr) {
                    f_ptr++;
                    f_ptr %= sizeof(in_memory_log);
                }
            }
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
            return 0;
        } else if (r != 0 && r != 9) {
            lprintf(LOG_ERROR, "Crash detected in squire desktop\n");

            crash_handler_config_t conf;
            FILE *f = fopen(CONFIG_FILE, "r");
            int s = read_config(&conf, f);
            if (f != NULL) {
                fclose(f);
            }

            if (!s) {
                lprintf(LOG_WARNING, "No valid configuration found, assuming that crash reports are off\n");
            }

            int report = conf.send_crash_report;
            free_config(&conf);

            if (report) {
                lprintf(LOG_INFO, "Sending report...\n");

                char *log_buf_final = malloc(sizeof(log_buf_final) * (sizeof(in_memory_log) + 1 + strlen(MSG_START) + strlen(MSG_END)));
                if (log_buf_final == NULL) {
                    lprintf(LOG_ERROR, "Cannot alloc webhook buffer.\n");
                    return 1;
                }

                // Copy from the front to the buffer end.
                // Case 1 - non-full buffer - b_ptr - f_ptr
                // .... f ------> b .....
                // Case 2 - full buffer - sizeof * in_memory_log - f_ptr
                // ---> b f ------------>

                // j is the log_buf_final index, i is the in_memory_log index.
                size_t i = 0, j = strlen(MSG_START);
                strcpy(log_buf_final, MSG_START);
                for (i = f_ptr; i != b_ptr; i++, i %= sizeof(in_memory_log), j++) {
                    log_buf_final[j] = in_memory_log[i];
                }

                if (f_ptr == b_ptr) {
                    strcpy(log_buf_final + j, FAILED_START_MSG);
                    j += strlen(FAILED_START_MSG);
                } else {
                    log_buf_final[j++] = in_memory_log[b_ptr];

                }
                strcpy(log_buf_final + j, MSG_END);

                // Send the message log then free.
                if (!send_webhook(log_buf_final, DISCORD_WEBHOOK_URL)) {
                    lprintf(LOG_ERROR, "Cannot send crash report :(, please upload a log to github issues when you can\n)");
                } else {
                    lprintf(LOG_INFO, "Crash report sent successfully, the log file is still on disc.\n");
                }
                free(log_buf_final);
            }
            return 1;
        }
    }

    return 0;
}
