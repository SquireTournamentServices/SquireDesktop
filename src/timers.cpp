#include <stdlib.h>
#include <time.h>
#include "./timers.h"
#include "../testing_h/logger.h"

void init_timer(sq_timer_t *t, long duration, bool start_now)
{
    *t = SQ_TIMER_T_DEFAULT;
    t->duration = duration;
    t->interrupts_len = 0;

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

int pause_timer(sq_timer_t *t)
{
    long now = time(NULL);

    // 1 would mean that the list is empty.
    if (t->interrupts_len == 0) {
        t->interrupts = (sq_timer_interrupt_t *) malloc(sizeof * t->interrupts);
    } else {
        if (t->interrupts[t->interrupts_len - 1].end == SQ_TIMER_INTERRUPT_T_NO_END) {
            lprintf(LOG_ERROR, "Already paused\n");
            return 0;
        }

        t->interrupts = (sq_timer_interrupt_t *)
                        realloc(t->interrupts, sizeof * t->interrupts * (1 + t->interrupts_len));
    }

    if (t->interrupts == NULL) {
        lprintf(LOG_ERROR, "Cannot malloc\n");
        return 0;
    }

    size_t i = t->interrupts_len;
    t->interrupts_len++;
    t->interrupts[i].start = now;
    t->interrupts[i].end = SQ_TIMER_INTERRUPT_T_NO_END;
    return 1;
}

int resume_timer(sq_timer_t *t)
{
    if (t->interrupts == NULL || t->interrupts_len == 0) {
        lprintf(LOG_ERROR, "Nothing to resume\n");
        return 0;
    }

    if (t->interrupts_len > 0) {
        if (t->interrupts[t->interrupts_len - 1].end != SQ_TIMER_INTERRUPT_T_NO_END) {
            lprintf(LOG_ERROR, "Already resumed\n");
            return 0;
        }
    }

    t->interrupts[t->interrupts_len - 1].end = time(NULL);
    return 1;
}

long time_left(sq_timer_t *t)
{
    long now = time(NULL);
    long ret = t->duration - now - t->start_time;
    long offset = 0;
    for (size_t i = 0; i < t->interrupts_len; i++) {
        if (t->interrupts[i].end == SQ_TIMER_INTERRUPT_T_NO_END) {
            offset += now - t->interrupts[i].start;
        } else {
            offset += t->interrupts[i].end - t->interrupts[i].end;
        }
    }

    return ret + offset;
}
