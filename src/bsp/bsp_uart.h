#pragma once
#include "my_bsp_config.h"

typedef enum { UN, ING, END } state_t;

#ifdef UART_0_INST
#define RX_BUF_SIZE 1030
#define RX_TIMEOUT 5
typedef struct {
    uint8_t rx_buf[RX_BUF_SIZE];
    uint8_t tx_buf[32];
    uint16_t rx_size; // 长度
    uint8_t time_out; // 超时
    state_t rx_state; // 状态码
} uart_t;             /* 串口结构体 */

static inline void uart_send_char(char ch) {
    // 等待串口0空闲
    while (uart_is_busy(UART_0_INST) == true);
    DL_UART_Main_transmitData(UART_0_INST, (uint8_t)ch);
}

// 外部调用
#define bsp_uart_init()                                                                                                \
    do {                                                                                                               \
        clear_pendingirq(UART_0_INST_INT_IRQN);                                                                        \
        enable_interrupt(UART_0_INST_INT_IRQN);                                                                        \
        enable_interrupt(TIMER_0_INST_INT_IRQN);                                                                       \
    } while (0)
void uart_send(uint8_t* str, uint16_t size);
void uart_send_string(uint8_t* str);

extern uart_t u0;
#endif
