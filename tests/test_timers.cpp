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
    ASSERT(t.interrupts == NULL);
    ASSERT(t.interrupts_len == 0);
    free_timer(&t);

    init_timer(&t, DURATION, true);
    ASSERT(t.duration == DURATION);
    ASSERT(t.start_time != SQ_TIMER_T_NOT_STARTED);
    ASSERT(abs(t.start_time - time(NULL)) < 2); // Should have started about now.
    ASSERT(t.interrupts == NULL);
    ASSERT(t.interrupts_len == 0);
    free_timer(&t);

    return 1;
}

static int test_res_pause()
{
    sq_timer_t t;
    init_timer(&t, DURATION, false);
    ASSERT(t.duration == DURATION);
    ASSERT(t.start_time == SQ_TIMER_T_NOT_STARTED);
    ASSERT(t.interrupts == NULL);
    ASSERT(t.interrupts_len == 0);

    ASSERT(pause_timer(&t));
    ASSERT(t.interrupts_len == 1);
    ASSERT(!pause_timer(&t));
    ASSERT(t.interrupts_len == 1);

    ASSERT(resume_timer(&t));
    ASSERT(t.interrupts_len == 1);
    ASSERT(!resume_timer(&t));
    ASSERT(t.interrupts_len == 1);

    for (int i = 0; i < 1000; i++) {
        size_t c = 2 + i;

        ASSERT(pause_timer(&t));
        ASSERT(t.interrupts_len == c);
        ASSERT(!pause_timer(&t));
        ASSERT(t.interrupts_len == c);

        ASSERT(resume_timer(&t));
        ASSERT(t.interrupts_len == c);
        ASSERT(!resume_timer(&t));
        ASSERT(t.interrupts_len == c);
    }

    free_timer(&t);

    return 1;
}

SUB_TEST(test_timers, {&test_init, "Test init"},
{&test_res_pause, "Test pause and, resume"})

