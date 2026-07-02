#include "boot.h"
#include "bsp_uart.h"

void send_order_tips() {
    printf("--help\r\n");
    printf("\r[0]reset\r\n");
    printf("\r[1]update A\r\n");
    printf("\r[2]enter A\r\n");
    boot0.flag |= BOOT_ORDER_FLAG;
}
int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // 无\n打印
    SYSCFG_DL_init();
    bsp_uart_init();
    if (boot_enter(5000)) {
        send_order_tips();
    } else {
        printf("--enter default mode\r\n");
        jump_to_app(A_START_ADDR);
        send_order_tips();
    }

    while (1) {
        if (u0.rx_state == END && (boot0.flag & BOOT_ORDER_FLAG)) {
            u0.rx_state = UN;
            switch (u0.rx_buf[u0.rx_size - 1]) {
            case '0':
                NVIC_SystemReset();
                break;
            case '1':
                erase_flash_region(A_START_ADDR, A_PAGE * 1024);
                boot0.flag |= IAP_XMODE_FLAG;
                // boot0.flag &= ~BOOT_ORDER_FLAG; //  接收中断超时判断有误时 解除注解试试
                memset(u0.rx_buf, 0xff, RX_BUF_SIZE);
                u0.rx_size = 0;
                break;
            case '2':
                jump_to_app(A_START_ADDR);
                break;
            default:
                break;
            }
        }
        // xmodem 边接收 边处理
        if (xmodem_rec_bin()) {
            printf("flash finished\r\n");
            jump_to_app(A_START_ADDR);
        }
    }
}
