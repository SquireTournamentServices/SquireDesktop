#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "./test_coins.h"
#include "./test_config.h"
#include "./test_io_utils.h"
#include "../testing_h/testing.h"
#include "./sq_link_test.h"

int test_func()
{
    ASSERT(1 == 1);
    return 1;
}

void handler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, handler);

    lprintf(LOG_INFO, "Running tests for Squire Desktop %s for %s @ %s\n", VERSION, OS, REPO_URL);
    unit_test tests[] = {
        {&test_func, "Testing_h self test"},
        {&coins_cpp_test, "Coin cpp test"},
        {&config_cpp_tests, "Config cpp test"},
        {&io_utils_cpp_test, "IO utils cpp test"},
        {&sq_link_test, "SQ link test"}
    };

    int failed_tests = run_tests(tests, sizeof(tests) / sizeof(*tests), "Squire Desktop Tests");
    lprintf(LOG_INFO, "%ld/%ld tests failed.\n", failed_tests, sizeof(tests) / sizeof(*tests));
    return failed_tests > 0;
}

