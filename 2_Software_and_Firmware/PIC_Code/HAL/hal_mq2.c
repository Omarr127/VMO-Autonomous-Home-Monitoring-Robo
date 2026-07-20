#include "hal_mq2.h"
#include "../MCAL/mcal_adc.h"

void     HAL_MQ2_Init(void)  { MCAL_ADC_Init(); }
uint16_t HAL_MQ2_Read(void)  { return MCAL_ADC_Read(MQ2_ADC_CH); }
