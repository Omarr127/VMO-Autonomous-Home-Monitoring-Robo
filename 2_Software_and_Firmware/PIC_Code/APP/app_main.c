#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#include "../config.h"
#include "../MCAL/mcal_adc.h"
#include "../MCAL/mcal_uart.h"
#include "../MCAL/mcal_timer.h"
#include "../MCAL/mcal_pwm.h"
#include "../HAL/hal_motor.h"
#include "../HAL/hal_mq2.h"
#include "../HAL/hal_ultrasonic.h"
#include "../HAL/hal_dht11.h"
#include "../SERVICES/srv_scheduler.h"
#include "../SERVICES/srv_protocol.h"

#define APP_TASK_COUNT 4u
#if APP_TASK_COUNT > SCHED_MAX_TASKS
#  error "APP_TASK_COUNT exceeds SCHED_MAX_TASKS"
#endif

#define CMD_BUF_SIZE  8u
static volatile char    s_cmd_buf[CMD_BUF_SIZE];
static volatile uint8_t s_cmd_head = 0;
static volatile uint8_t s_cmd_tail = 0;

#define DEBUG_RX  1

static volatile uint8_t s_dbg_oerr = 0;
static volatile uint8_t s_dbg_ferr = 0;
static volatile char    s_dbg_raw  = 0;
static volatile uint8_t s_dbg_ign  = 0;
static volatile uint8_t s_dbg_pending = 0;

static inline void cmd_buf_push(char c) {
    uint8_t next = (uint8_t)((s_cmd_head + 1u) % CMD_BUF_SIZE);
    if (next != s_cmd_tail) {
        s_cmd_buf[s_cmd_head] = c;
        s_cmd_head = next;
    }
}

static inline uint8_t cmd_buf_empty(void) { return s_cmd_head == s_cmd_tail; }

static inline char cmd_buf_pop(void) {
    char c = s_cmd_buf[s_cmd_tail];
    s_cmd_tail = (uint8_t)((s_cmd_tail + 1u) % CMD_BUF_SIZE);
    return c;
}

#define GAS_DEBOUNCE_COUNT  3u
static uint8_t s_gas_high_count = 0;
static uint8_t s_gas_locked     = 0;

static void update_gas_lock(void) {
    if (g_sensors.gas_raw > GAS_DANGER_LEVEL) {
        if (s_gas_high_count < GAS_DEBOUNCE_COUNT) s_gas_high_count++;
        if (s_gas_high_count >= GAS_DEBOUNCE_COUNT) s_gas_locked = 1;
    } else {
        s_gas_high_count = 0;
        s_gas_locked     = 0;
    }
}

static void execute_command(char cmd) {
    static char s_last_cmd = 'X';
    if (cmd == s_last_cmd && cmd != 'P' && !(cmd >= '0' && cmd <= '9')) return;
    s_last_cmd = cmd;

    if (cmd == 'P') {
        LED_HEARTBEAT = ~LED_HEARTBEAT;
    } else if (cmd >= 'a' && cmd <= 'p') {
        uint8_t val = cmd - 'a';
        uint8_t in1 = (val & 0x08) ? 1 : 0;
        uint8_t in2 = (val & 0x04) ? 1 : 0;
        uint8_t in3 = (val & 0x02) ? 1 : 0;
        uint8_t in4 = (val & 0x01) ? 1 : 0;
        HAL_Motor_ExecuteRaw(in1, in2, in3, in4);
    } else if (cmd >= '0' && cmd <= '9') {
        // Speed control: 0 to 9 mapped to PWM duty cycle (0 to 621, Max PR2 is 624)
        uint16_t speed = (cmd - '0') * 69;
        MCAL_PWM_SetLeft(speed);
        MCAL_PWM_SetRight(speed);
    }
}

void __interrupt() isr(void) {
    if (INTCONbits.T0IE && INTCONbits.T0IF) {
        MCAL_Timer0_ISR_Handler();
    }

    if (PIE1bits.RCIE && PIR1bits.RCIF) {
        char c;

        if (RCSTAbits.OERR) {
            char c1 = RCREG;
            char c2 = RCREG;
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;

            s_dbg_oerr = 1;
            s_dbg_pending = 1;

            if ((c1 >= 'a' && c1 <= 'p') || (c1 >= '0' && c1 <= '9') || c1 == 'P') {
                    cmd_buf_push(c1);
                    s_dbg_ign = 0;
                } else {
                    s_dbg_ign = 1;
                }
            if ((c2 >= 'a' && c2 <= 'p') || (c2 >= '0' && c2 <= '9') || c2 == 'P') {
                    cmd_buf_push(c2);
                    s_dbg_ign = 0;
                } else {
                    s_dbg_ign = 1;
                }
        }
        else if (RCSTAbits.FERR) {
            c = RCREG;
            (void)c;

            s_dbg_ferr = 1;
            s_dbg_pending = 1;
        }
        else {
            c = RCREG;

            s_dbg_raw = c;
            s_dbg_pending = 1;

            if ((c >= 'a' && c <= 'p') || c == 'P') {
                cmd_buf_push(c);
                s_dbg_ign = 0;
            } else {
                s_dbg_ign = 1;
            }
        }
    }
}

static void task_adc(void) {
    g_sensors.gas_raw = MCAL_ADC_Read(MQ2_ADC_CH);
    update_gas_lock();
}

static void task_ultrasonic(void) {
    g_sensors.dist_rear_cm = HAL_Ultrasonic_ReadRear();
}

static void task_dht11(void) {
    uint16_t t = g_sensors.temperature_x10;
    uint16_t h = g_sensors.humidity_x10;
    if (HAL_DHT11_Read(&t, &h) == DHT11_OK) {
        g_sensors.temperature_x10 = t;
        g_sensors.humidity_x10    = h;
    }
}

static void task_uart_tx(void) {
    SRV_Protocol_SendPacket();
}

static void task_debug_rx(void) {
#if DEBUG_RX
    uint8_t oerr, ferr, ign, pending;
    char raw;

    INTCONbits.GIE = 0;
    oerr    = s_dbg_oerr;
    ferr    = s_dbg_ferr;
    raw     = s_dbg_raw;
    ign     = s_dbg_ign;
    pending = s_dbg_pending;
    s_dbg_oerr = s_dbg_ferr = s_dbg_ign = s_dbg_pending = 0;
    INTCONbits.GIE = 1;

    if (!pending) return;

    if (oerr) {
        MCAL_UART_WriteString("RX:ERR_OERR\r\n");
    } else if (ferr) {
        MCAL_UART_WriteString("RX:ERR_FERR\r\n");
    } else if (ign) {
        MCAL_UART_WriteString("RX:IGN:");
        MCAL_UART_WriteChar(raw);
        MCAL_UART_WriteString("\r\n");
    } else {
        MCAL_UART_WriteString("RX:");
        MCAL_UART_WriteChar(raw);
        MCAL_UART_WriteString("\r\n");
    }
#endif
}

void main(void) {
    TRISB = 0b00001001;
    TRISC = 0b10000000;
    TRISD = 0x00u;
    TRISA = 0xFFu;
    TRISE = 0x07u;
    PORTB = 0x00u;
    PORTC = 0x00u;
    PORTD = 0x00u;

    MCAL_ADC_Init();
    MCAL_UART_Init();
    MCAL_Timer0_Init();
    MCAL_Timer1_Init();
    MCAL_PWM_Init();
    HAL_Motor_Init();
    HAL_MQ2_Init();
    HAL_Ultrasonic_Init();

    SRV_Scheduler_Init();
    SRV_Scheduler_AddTask(task_adc,        TASK_ADC_PERIOD_MS);
    SRV_Scheduler_AddTask(task_ultrasonic, TASK_ULTRASONIC_PERIOD_MS);
    SRV_Scheduler_AddTask(task_dht11,      TASK_DHT11_PERIOD_MS);
    SRV_Scheduler_AddTask(task_uart_tx,    TASK_UART_TX_PERIOD_MS);

    PIE1bits.RCIE   = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE  = 1;

    __delay_ms(500);
    MCAL_UART_WriteString("BOOT OK\r\n");

    g_sensors.temperature_x10 = 0;
    g_sensors.humidity_x10    = 0;
    g_sensors.gas_raw         = 0;
    g_sensors.dist_rear_cm    = 999;

    while (1) {
        while (!cmd_buf_empty()) {
            execute_command(cmd_buf_pop());
        }

        task_debug_rx();

        SRV_Scheduler_Run();
    }
}
