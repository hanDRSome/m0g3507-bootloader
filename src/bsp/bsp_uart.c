#include "bsp_uart.h"

#ifdef UART_0_INST
uart_t u0;

void uart_send(uint8_t* str, uint16_t size) {
    while (size--) {
        uart_send_char(*str++);
    }
}

void uart_send_string(uint8_t* str) {
    while (*str != '\0') {
        uart_send_char(*str++);
    }
}

// 重定义 printf
int _write(int fd, char* ptr, int len) {
    for (int i = 0; i < len; i++) {
        uart_send_char(ptr[i]);
    }
    return len;
}

#endif