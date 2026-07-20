#ifndef SRV_PROTOCOL_H
#define SRV_PROTOCOL_H

#include "../config.h"

typedef struct {
    uint16_t temperature_x10;
    uint16_t humidity_x10;
    uint16_t dist_rear_cm;
    uint16_t gas_raw;
} SensorData_t;

extern SensorData_t g_sensors;

void SRV_Protocol_SendPacket(void);

#endif
