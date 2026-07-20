#include "hal_dht11.h"

#define DHT_TIMEOUT 1000u

static uint8_t dht_buf[5];

DHT11_Status_t HAL_DHT11_Read(uint16_t *temp, uint16_t *hum) {
    uint8_t  bits[40];
    uint16_t count;
    uint8_t  i, j;
    uint8_t  gie_saved;

    for (i = 0; i < 5; i++) dht_buf[i] = 0;

    gie_saved = INTCONbits.GIE;
    INTCONbits.GIE = 0;

    DHT11_TRIS = 0;
    DHT11_PIN  = 0; __delay_ms(18);
    DHT11_TRIS = 1; __delay_us(30);

    count = 0; while (DHT11_PIN)  { __delay_us(1); if (++count > DHT_TIMEOUT) { INTCONbits.GIE = gie_saved; return DHT11_ERR_TIMEOUT; } }
    count = 0; while (!DHT11_PIN) { __delay_us(1); if (++count > DHT_TIMEOUT) { INTCONbits.GIE = gie_saved; return DHT11_ERR_TIMEOUT; } }
    count = 0; while (DHT11_PIN)  { __delay_us(1); if (++count > DHT_TIMEOUT) { INTCONbits.GIE = gie_saved; return DHT11_ERR_TIMEOUT; } }

    for (i = 0; i < 40; i++) {
        count = 0;
        while (!DHT11_PIN) { __delay_us(1); if (++count > DHT_TIMEOUT) { INTCONbits.GIE = gie_saved; return DHT11_ERR_TIMEOUT; } }
        __delay_us(40);
        bits[i] = DHT11_PIN ? 1u : 0u;
        if (bits[i]) {
            count = 0;
            while (DHT11_PIN) { __delay_us(1); if (++count > DHT_TIMEOUT) { INTCONbits.GIE = gie_saved; return DHT11_ERR_TIMEOUT; } }
        }
    }

    INTCONbits.GIE = gie_saved;

    for (i = 0; i < 5; i++) {
        dht_buf[i] = 0;
        for (j = 0; j < 8; j++) dht_buf[i] = (uint8_t)((dht_buf[i] << 1) | bits[i * 8 + j]);
    }

    if (dht_buf[0] == 0 && dht_buf[2] == 0) return DHT11_ERR_NO_DATA;
    if ((uint8_t)(dht_buf[0] + dht_buf[1] + dht_buf[2] + dht_buf[3]) != dht_buf[4])
        return DHT11_ERR_CHECKSUM;

    *hum  = (uint16_t)(dht_buf[0] * 10u + dht_buf[1]);
    *temp = (uint16_t)(dht_buf[2] * 10u + dht_buf[3]);

    return DHT11_OK;
}
