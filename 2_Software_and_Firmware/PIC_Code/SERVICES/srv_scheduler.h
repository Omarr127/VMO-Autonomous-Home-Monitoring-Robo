#ifndef SRV_SCHEDULER_H
#define SRV_SCHEDULER_H

#include "../config.h"

#define SCHED_MAX_TASKS 4u

typedef void (*TaskFunc_t)(void);

typedef struct {
    TaskFunc_t func;
    uint32_t   period_ms;
    uint32_t   last_run_ms;
    uint8_t    enabled;
} SchedTask_t;

void   SRV_Scheduler_Init(void);
int8_t SRV_Scheduler_AddTask(TaskFunc_t func, uint32_t period_ms);
void   SRV_Scheduler_Run(void);

#endif
