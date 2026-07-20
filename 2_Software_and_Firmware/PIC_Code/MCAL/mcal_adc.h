#ifndef MCAL_ADC_H
#define MCAL_ADC_H

#include "../config.h"

void     MCAL_ADC_Init(void);
uint16_t MCAL_ADC_Read(uint8_t channel);

#endif
