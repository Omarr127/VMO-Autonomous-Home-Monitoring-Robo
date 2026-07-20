#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define _XTAL_FREQ  20000000UL

typedef enum { STD_OK = 0, STD_ERR = 1, STD_TIMEOUT = 2 } Std_ReturnType;

#define TMR0_RELOAD         100u
#define UART_SPBRG_VALUE    129u
#define PWM_PR2_VALUE       155u
#define PWM_T2CON_VALUE     0x07u

#define MQ2_ADC_CH          0u

#define DHT11_PIN           PORTBbits.RB0
#define DHT11_TRIS          TRISBbits.TRISB0

#define TRIG_REAR           PORTBbits.RB2
#define TRIG_REAR_TRIS      TRISBbits.TRISB2
#define ECHO_REAR           PORTBbits.RB3
#define ECHO_REAR_TRIS      TRISBbits.TRISB3

#define LED_HEARTBEAT       PORTBbits.RB1
#define LED_TX              PORTBbits.RB4
#define LED_RX              PORTBbits.RB5

#define M_IN1               PORTDbits.RD0
#define M_IN2               PORTDbits.RD1
#define M_IN3               PORTCbits.RC0
#define M_IN4               PORTCbits.RC3

#define DIST_OUT_OF_RANGE   999u
#define GAS_DANGER_LEVEL    400u

#define TASK_ADC_PERIOD_MS          100u
#define TASK_ULTRASONIC_PERIOD_MS   200u
#define TASK_DHT11_PERIOD_MS       2000u
#define TASK_UART_TX_PERIOD_MS      250u
#define TASK_HEARTBEAT_PERIOD_MS    500u

#endif
