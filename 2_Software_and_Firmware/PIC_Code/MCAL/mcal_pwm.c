#include "mcal_pwm.h"

void MCAL_PWM_Init(void) {
    PR2     = 155;
    CCP1CON = 0x0C;
    CCP2CON = 0x0C;
    T2CON   = 0x07;
}

void MCAL_PWM_SetLeft(uint16_t duty) {
    CCPR2L  = (uint8_t)(duty >> 2);
    CCP2CON = (CCP2CON & 0xCF) | (uint8_t)((duty & 0x03) << 4);
}

void MCAL_PWM_SetRight(uint16_t duty) {
    CCPR1L  = (uint8_t)(duty >> 2);
    CCP1CON = (CCP1CON & 0xCF) | (uint8_t)((duty & 0x03) << 4);
}
