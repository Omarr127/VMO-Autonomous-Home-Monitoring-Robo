#ifndef MCAL_TIMER_H
#define MCAL_TIMER_H

#include "../config.h"

extern volatile uint32_t g_tick_ms;

void     MCAL_Timer0_Init(void);
void     MCAL_Timer0_ISR_Handler(void);
void     MCAL_Timer1_Init(void);
void     MCAL_Timer1_Start(void);
void     MCAL_Timer1_Stop(void);
uint16_t MCAL_Timer1_Read(void);
uint32_t MCAL_GetTick(void);

#endif
