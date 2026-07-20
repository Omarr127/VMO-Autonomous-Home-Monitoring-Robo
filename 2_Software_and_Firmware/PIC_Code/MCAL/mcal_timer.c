#include "mcal_timer.h"

volatile uint32_t g_tick_ms = 0;

void MCAL_Timer0_Init(void) {
    OPTION_REG = (OPTION_REG & 0xC0) | 0x04;
    TMR0 = TMR0_RELOAD;
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
}

void MCAL_Timer0_ISR_Handler(void) {
    INTCONbits.T0IF = 0;
    TMR0 = TMR0_RELOAD;
    g_tick_ms++;
}

void MCAL_Timer1_Init(void) {
    T1CON = 0b00010000;
}

void MCAL_Timer1_Start(void) {
    TMR1H = 0; TMR1L = 0;
    T1CONbits.TMR1ON = 1;
}

void MCAL_Timer1_Stop(void) {
    T1CONbits.TMR1ON = 0;
}

uint16_t MCAL_Timer1_Read(void) {
    uint8_t lo = TMR1L;
    uint8_t hi = TMR1H;
    return (uint16_t)((uint16_t)hi << 8) | lo;
}

uint32_t MCAL_GetTick(void) {
    uint32_t t;
    INTCONbits.GIE = 0;
    t = g_tick_ms;
    INTCONbits.GIE = 1;
    return t;
}
