#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "./test_coins.h"
#include "./test_config.h"
#include "./test_utils.h"
#include "./test_filter_list.h"
#include "./test_timers.h"
#include "../testing_h/testing.h"
#include "./unit_tests.h"

static int test_func()
{
    ASSERT(1 == 1);
    return 1;
}

int unit_tests()
{
    lprintf(LOG_INFO, "Running tests for Squire Desktop %s for %s @ %s\n", VERSION, OS, REPO_URL);
    unit_test tests[] = {
        {&test_func, "Testing_h self test"},
        {&coins_cpp_test, "Coin cpp test"},
        {&config_cpp_tests, "Config cpp test"},
        {&utils_cpp_test, "IO utils cpp test"},
        {&filter_list_tests, "Filter list cpp test"},
        {&test_timers, "Timers cpp test"},
    };

    int failed_tests = run_tests(tests, sizeof(tests) / sizeof(*tests), "Squire Desktop Tests");
    lprintf(LOG_INFO, "%d/%ld tests failed.\n", failed_tests, sizeof(tests) / sizeof(*tests));
    return failed_tests > 0;
}
