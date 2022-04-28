#include <stdlib.h>
#include "./test_coins.h"
#include "./test_config.h"
#include "./test_io_utils.h"
#include "../testing_h/testing.h"

int test_func()
{
    ASSERT(1 == 1);
    return 1;
}

int main()
{
    lprintf(LOG_INFO, "Running tests for Squire Desktop %s for %s @ %s\n", VERSION, OS, REPO_URL);
    unit_test tests[] = {
        {&test_func, "Testing_h self test"},
        {&coins_cpp_test, "Coin cpp test"},
        {&config_cpp_tests, "Config cpp test"},
        {&io_utils_cpp_test, "IO utils cpp test"}
    };

    int failed_tests = run_tests(tests, sizeof(tests) / sizeof(*tests), "Squire Desktop Tests");
    lprintf(LOG_INFO, "%d/%d tests failed.\n", failed_tests, sizeof(tests) / sizeof(*tests));
    return failed_tests > 0;
}

