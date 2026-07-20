#include "mcal_adc.h"

void MCAL_ADC_Init(void) {
    ADCON1 = 0b10001110;
    ADCON0 = 0b10000001;
}

uint16_t MCAL_ADC_Read(uint8_t channel) {
    ADCON0 = (ADCON0 & 0b11000101) | (uint8_t)(channel << 3);
    __delay_us(20);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return (uint16_t)(((uint16_t)ADRESH << 8) | ADRESL);
}
