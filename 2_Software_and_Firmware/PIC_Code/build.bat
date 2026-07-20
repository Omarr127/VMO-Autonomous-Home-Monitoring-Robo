@echo off
setlocal
echo =====================================================
echo  PIC16F877A Firmware Build
echo =====================================================

if not exist output mkdir output

xc8-cc -mcpu=16F877A -O2 -mdfp=.\PIC16F_DFP\xc8 ^
    MCAL\mcal_timer.c     ^
    MCAL\mcal_adc.c       ^
    MCAL\mcal_uart.c      ^
    MCAL\mcal_pwm.c       ^
    HAL\hal_motor.c       ^
    HAL\hal_mq2.c         ^
    HAL\hal_ultrasonic.c  ^
    HAL\hal_dht11.c       ^
    SERVICES\srv_scheduler.c ^
    SERVICES\srv_protocol.c  ^
    APP\app_main.c        ^
    -o output\Driver.hex

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] output\Driver.hex is ready.
) else (
    echo.
    echo [FAILED] Fix the errors above.
    exit /b 1
)
endlocal
