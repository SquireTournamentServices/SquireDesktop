#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include "../testing_h/testing.h"
#include "./sq_link_test.h"
#include "./test_tournament_ffi.h"
#include "./test_round_ffi.h"
#include "./test_ffi.h"
#include <squire_ffi/squire_ffi.h>

int test_func()
{
    ASSERT(1 == 1);
    return 1;
}

int ffi_tests()
{
    lprintf(LOG_INFO, "Running tests for Squire Desktop FFI %s for %s @ %s\n", VERSION, OS, REPO_URL);
    unit_test tests[] = {
        {&test_func, "Testing_h self test"},
        {&sq_link_test, "SQ Link Test (debug test)"},
        {&test_tournament_ffi, "Tournament FFI Tests"},
        {&test_round_ffi, "Round FFI Tests"}
    };

    squire_ffi::init_squire_ffi(); // This is important!
    int failed_tests = run_tests(tests, sizeof(tests) / sizeof(*tests), "Squire Desktop Tests");
    lprintf(LOG_INFO, "%d/%ld tests failed.\n", failed_tests, sizeof(tests) / sizeof(*tests));
    return failed_tests > 0;

}
