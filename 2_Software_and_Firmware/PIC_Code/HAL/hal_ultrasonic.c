#include "hal_ultrasonic.h"
#include <xc.h>

void HAL_Ultrasonic_Init(void) {
    TRIG_REAR_TRIS = 0; // Output
    ECHO_REAR_TRIS = 1; // Input
    TRIG_REAR = 0;
}

uint16_t HAL_Ultrasonic_ReadRear(void) {
    uint16_t time = 0;
    uint16_t timeout;

    TRIG_REAR = 1;
    __delay_us(10);
    TRIG_REAR = 0;

    // Wait for Echo to go HIGH, but timeout if it takes too long
    timeout = 0;
    while (!ECHO_REAR) {
        if (++timeout > 10000) return DIST_OUT_OF_RANGE;
    }

    // Measure the length of the Echo HIGH pulse, with a timeout
    while (ECHO_REAR) {
        time++;
        __delay_us(1);
        if (time > 25000) return DIST_OUT_OF_RANGE;
    }

    // Convert time to distance in CM (calibrate this divisor if needed)
    return time / 58;
}
