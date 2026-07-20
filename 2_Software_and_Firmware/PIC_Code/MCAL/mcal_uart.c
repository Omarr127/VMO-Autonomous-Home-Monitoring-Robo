#include "mcal_uart.h"

void MCAL_UART_Init(void) {
    TXSTAbits.BRGH = 1;
    SPBRG = UART_SPBRG_VALUE;
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
}

void MCAL_UART_CheckError(void) {
    if (RCSTAbits.OERR) {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }
}

char MCAL_UART_DataReady(void) { return PIR1bits.RCIF; }

char MCAL_UART_Read(void) {
    while (!PIR1bits.RCIF);
    return RCREG;
}

void MCAL_UART_WriteChar(char c) {
    while (!TXSTAbits.TRMT);
    TXREG = c;
}

void MCAL_UART_WriteString(const char *str) {
    for (int k = 0; str[k] != '\0'; k++)
        MCAL_UART_WriteChar(str[k]);
}
