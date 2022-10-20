#include "./test_timers.h"
#include "../src/timers.h"
#include <time.h>

#define DURATION 180

static int test_init()
{
    sq_timer_t t;
    init_timer(&t, DURATION, false);
    ASSERT(t.duration == DURATION);
    ASSERT(t.start_time == SQ_TIMER_T_NOT_STARTED);
    free_timer(&t);

    init_timer(&t, DURATION, true);
    ASSERT(t.duration == DURATION);
    ASSERT(t.start_time != SQ_TIMER_T_NOT_STARTED);
    ASSERT(abs(t.start_time - time(NULL)) < 2); // Should have started about now.
    free_timer(&t);

    return 1;
}

SUB_TEST(test_timers, {&test_init, "Test init"})

