#pragma once

#define SQ_TIMER_INTERRUPT_T_NO_END -1
#define SQ_TIMER_INTERRUPT_T_DEFAULT {time(NULL), SQ_TIMER_INTERRUPT_T_NO_END}

typedef struct sq_timer_interrupt_t {
    long start;
    long end;
} sq_timer_interrupt_t;

#define SQ_TIMER_T_DEFAULT {0, SQ_TIMER_T_NOT_STARTED, 0, NULL}
#define SQ_TIMER_T_NOT_STARTED -1
typedef struct sq_timer_t {
    long duration;
    long start_time;
    int interrupts_len;
    sq_timer_interrupt_t *interrupts;
} sq_timer_t;

void init_timer(sq_timer_t *t, long duration, bool start_now);
void free_timer(sq_timer_t *t);
void pause_timer(sq_timer_t *t);
void resume_timer(sq_timer_t *t);

