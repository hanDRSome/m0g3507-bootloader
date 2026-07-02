#pragma once
#include "stdio.h"
#include "string.h"
#include "ti_msp_dl_config.h"

#define gpio_set(port, pin) DL_GPIO_setPins(port, pin)
#define gpio_clear(port, pin) DL_GPIO_clearPins(port, pin)
#define gpio_toggle(port, pin) DL_GPIO_togglePins(port, pin)
#define gpio_read(port, pin) DL_GPIO_readPins(port, pin)
#define uart_is_busy(uart) DL_UART_isBusy(uart)
#define start_timer(gptimer) DL_Timer_startCounter(gptimer)
#define enable_interrupt(IRQn) NVIC_EnableIRQ(IRQn)
#define clear_pendingirq(IRQn) NVIC_ClearPendingIRQ(IRQn)
#define delay_us(xus) delay_cycles(xus* CPUCLK_FREQ / 1000000)
#define timer_start(IRQn) DL_Timer_startCounter(IRQn)
#define timer_stop(IRQn) DL_Timer_stopCounter(IRQn)
