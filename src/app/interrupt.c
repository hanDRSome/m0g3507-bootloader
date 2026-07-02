#include "boot.h"
#include "bsp_uart.h"


#ifdef UART_0_INST_IRQHandler
void UART_0_INST_IRQHandler(void) {

    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
    case DL_UART_MAIN_IIDX_RX:
        while (DL_UART_receiveDataCheck(UART_0_INST, (uint8_t*)u0.rx_buf + u0.rx_size)) {
            u0.rx_size++;
            if (u0.rx_size >= RX_BUF_SIZE)
                u0.rx_size = 0;
        }
        u0.time_out = 1;
        break;
    default:
        break;
    }
}

void TIMER_0_INST_IRQHandler(void) {

    if (DL_TimerA_getPendingInterrupt(TIMER_0_INST) == DL_TIMER_IIDX_ZERO) {
        if (boot0.flag & IAP_XMODE_FLAG) {
            boot0.time_out++;
            if (boot0.time_out >= BOOT_TIME_OUT)
                boot0.time_out = BOOT_TIME_OUT;
        }
        if (u0.time_out > 0) {
            u0.time_out++;
            if (u0.time_out >= RX_TIMEOUT) {
                u0.time_out = 0;
                u0.rx_state = END;
            }
        }
    }
}
#endif