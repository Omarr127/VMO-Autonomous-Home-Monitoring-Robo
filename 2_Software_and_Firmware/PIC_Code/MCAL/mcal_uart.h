#ifndef MCAL_UART_H
#define MCAL_UART_H

#include <xc.h>
#include "../config.h"

void MCAL_UART_Init(void);
void MCAL_UART_CheckError(void);
char MCAL_UART_DataReady(void);
char MCAL_UART_Read(void);
void MCAL_UART_WriteChar(char c);
void MCAL_UART_WriteString(const char *str);

#endif
