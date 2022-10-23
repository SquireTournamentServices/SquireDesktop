#include "./test_timers.h"
#include "../src/timers.h"
#include <time.h>
#include <unistd.h>

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

#define MAX 5

static int test_time_left()
{
    sq_timer_t t;
    init_timer(&t, DURATION, true);
    ASSERT(t.duration == DURATION);
    ASSERT(t.start_time != SQ_TIMER_T_NOT_STARTED);
    ASSERT(abs(t.start_time - time(NULL)) < 2); // Should have started about now.
    ASSERT(t.interrupts == NULL);
    ASSERT(t.interrupts_len == 0);
    ASSERT(time_left(&t) < 2 + DURATION); // Should have started about now.

    for (int i = 0; i < MAX; i++) {
        lprintf(LOG_INFO, "Testing resume, pause and, time left sorry. %ds left\n", MAX - i);
        ASSERT(pause_timer(&t));
        sleep(1);
        ASSERT(resume_timer(&t));

        ASSERT(time_left(&t) < 2 + DURATION - (i + 1)); // Should have started about now.
    }

    free_timer(&t);
    return 1;
}

SUB_TEST(test_timers, {&test_init, "Test init"},
{&test_res_pause, "Test pause and, resume"},
{&test_time_left, "Test time_left"})

