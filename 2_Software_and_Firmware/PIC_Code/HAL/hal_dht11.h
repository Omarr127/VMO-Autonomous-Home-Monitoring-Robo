#ifndef HAL_DHT11_H
#define HAL_DHT11_H

#include "../config.h"

typedef enum {
    DHT11_OK           = 0,
    DHT11_ERR_TIMEOUT  = 1,
    DHT11_ERR_CHECKSUM = 2,
    DHT11_ERR_NO_DATA  = 3
} DHT11_Status_t;

DHT11_Status_t HAL_DHT11_Read(uint16_t *temperature_x10, uint16_t *humidity_x10);

#endif
