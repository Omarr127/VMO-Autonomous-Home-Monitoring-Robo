#ifndef MCAL_PWM_H
#define MCAL_PWM_H

#include "../config.h"

void MCAL_PWM_Init(void);
void MCAL_PWM_SetLeft(uint16_t duty);
void MCAL_PWM_SetRight(uint16_t duty);

#endif
