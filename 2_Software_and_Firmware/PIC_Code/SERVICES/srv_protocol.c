#include "srv_protocol.h"
#include "../MCAL/mcal_uart.h"

SensorData_t g_sensors = {0, 0, 999, 0};

static void write_u16(uint16_t v) {
    char    buf[5];
    uint8_t n = 0;
    if (v == 0) { MCAL_UART_WriteChar('0'); return; }
    while (v) { buf[n++] = (char)('0' + (v % 10u)); v /= 10u; }
    while (n) MCAL_UART_WriteChar(buf[--n]);
}

static void write_hex8(uint8_t v) {
    static const char H[] = "0123456789ABCDEF";
    MCAL_UART_WriteChar(H[v >> 4]);
    MCAL_UART_WriteChar(H[v & 0x0F]);
}

static uint8_t xor_str(const char *s) {
    uint8_t cs = 0;
    while (*s) cs ^= (uint8_t)*s++;
    return cs;
}

void SRV_Protocol_SendPacket(void) {
    static char p[40];
    uint8_t i = 0;
    uint8_t cs;

    p[i++] = 'T';
    { uint16_t v = g_sensors.temperature_x10;
      char t[5]; uint8_t n = 0;
      if (!v) t[n++]='0'; else { uint16_t x=v; while(x){t[n++]=(char)('0'+x%10u);x/=10u;} }
      while(n) p[i++]=t[--n]; }
    p[i++]=','; p[i++]='H';
    { uint16_t v = g_sensors.humidity_x10;
      char t[5]; uint8_t n=0;
      if(!v) t[n++]='0'; else {uint16_t x=v; while(x){t[n++]=(char)('0'+x%10u);x/=10u;}}
      while(n) p[i++]=t[--n]; }
    p[i++]=','; p[i++]='R';
    { uint16_t v = g_sensors.dist_rear_cm;
      char t[5]; uint8_t n=0;
      if(!v) t[n++]='0'; else {uint16_t x=v; while(x){t[n++]=(char)('0'+x%10u);x/=10u;}}
      while(n) p[i++]=t[--n]; }
    p[i++]=','; p[i++]='L'; p[i++]='0';
    p[i++]=','; p[i++]='X'; p[i++]='0';
    p[i++]=','; p[i++]='G';
    { uint16_t v = g_sensors.gas_raw;
      char t[5]; uint8_t n=0;
      if(!v) t[n++]='0'; else {uint16_t x=v; while(x){t[n++]=(char)('0'+x%10u);x/=10u;}}
      while(n) p[i++]=t[--n]; }
    p[i] = '\0';

    cs = xor_str(p);

    MCAL_UART_WriteChar('<');
    MCAL_UART_WriteString(p);
    MCAL_UART_WriteChar('|');
    write_hex8(cs);
    MCAL_UART_WriteChar('>');
    MCAL_UART_WriteChar('\r');
    MCAL_UART_WriteChar('\n');
}
