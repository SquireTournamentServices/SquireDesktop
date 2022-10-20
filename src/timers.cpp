#include <stdlib.h>
#include <time.h>
#include "./timers.h"

void init_timer(sq_timer_t *t, long duration, bool start_now)
{
    *t = SQ_TIMER_T_DEFAULT;
    t->duration = duration;

    if (start_now) {
        t->start_time = time(NULL);
    }
}

void free_timer(sq_timer_t *t)
{
    if (t->interrupts != NULL) {
        free(t->interrupts);
    }
}

void pause_timer(sq_timer_t *t)
{

}

void resume_timer(sq_timer_t *t)
{

}

