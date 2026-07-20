#ifndef HAL_MOTOR_H
#define HAL_MOTOR_H

#include "../config.h"

void HAL_Motor_Init(void);
void HAL_Motor_ExecuteRaw(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4);

#endif
