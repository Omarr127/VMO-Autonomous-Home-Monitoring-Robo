#include "srv_scheduler.h"
#include "../MCAL/mcal_timer.h"

static SchedTask_t s_tasks[SCHED_MAX_TASKS];
static uint8_t     s_count = 0;

void SRV_Scheduler_Init(void) {
    s_count = 0;
    uint8_t i;
    for (i = 0; i < SCHED_MAX_TASKS; i++) {
        s_tasks[i].func       = 0;
        s_tasks[i].period_ms  = 0;
        s_tasks[i].last_run_ms = 0;
        s_tasks[i].enabled    = 0;
    }
}

int8_t SRV_Scheduler_AddTask(TaskFunc_t func, uint32_t period_ms) {
    if (s_count >= SCHED_MAX_TASKS || !func) return -1;
    s_tasks[s_count].func        = func;
    s_tasks[s_count].period_ms   = period_ms;
    s_tasks[s_count].last_run_ms = MCAL_GetTick();
    s_tasks[s_count].enabled     = 1;
    return (int8_t)(s_count++);
}

void SRV_Scheduler_Run(void) {
    uint8_t  i;
    uint32_t now = MCAL_GetTick();
    for (i = 0; i < s_count; i++) {
        if (!s_tasks[i].enabled) continue;
        if ((now - s_tasks[i].last_run_ms) >= s_tasks[i].period_ms) {
            s_tasks[i].last_run_ms = now;
            s_tasks[i].func();
        }
    }
}
