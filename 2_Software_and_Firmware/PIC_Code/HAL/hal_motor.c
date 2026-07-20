#include "hal_motor.h"
#include "../MCAL/mcal_pwm.h"

static uint8_t s_portc = 0x00u;

#define IN3_BIT  0x01u
#define IN4_BIT  0x08u
#define PWM_MAX  624u // 100% Duty cycle based on PR2=155

static void set_portc_motor(uint8_t in3, uint8_t in4) {
    s_portc &= (uint8_t)~(IN3_BIT | IN4_BIT);
    if (in3) s_portc |= IN3_BIT;
    if (in4) s_portc |= IN4_BIT;
    PORTC = s_portc;
}

void HAL_Motor_Init(void) {
    s_portc = 0x00u;
    PORTC = s_portc;
    PORTD = 0x00u;
    MCAL_PWM_Init();

    // Set to 100% Full Throttle by default and leave it there
    MCAL_PWM_SetLeft(PWM_MAX);
    MCAL_PWM_SetRight(PWM_MAX);
}

void HAL_Motor_ExecuteRaw(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4) {
    PORTDbits.RD0 = in1;
    PORTDbits.RD1 = in2;
    set_portc_motor(in3, in4);
}
