#include "boot.h"
#include "bsp_uart.h"
boot_t boot0;

/**
 * @brief 判断擦除区是否为0xff
 *
 */
__attribute__((section(".ramfunc"))) static bool is_sector_erased(uint32_t addr) {
    for (uint32_t p = addr; p < addr + FLASH_ONE_PAGE_SIZE; p += 4) {
        if (*(volatile uint32_t*)p != 0xFFFFFFFF) {
            return false;
        }
    }
    return true;
}
/**
 * @brief erace flash
 *
 * @param startAddr
 * @param size
 * @return true
 * @return false
 */
__attribute__((section(".ramfunc"))) bool erase_flash_region(uint32_t startAddr, uint32_t size) {

    if (startAddr % FLASH_ONE_PAGE_SIZE != 0) {
        return false;
    }

    // 逐扇区处理
    for (uint32_t addr = startAddr; addr < startAddr + size; addr += FLASH_ONE_PAGE_SIZE) {
        unprotect_flash();

        if (is_sector_erased(addr)) {
            // 为0xff 跳过erase
            continue;
        }

        DL_FLASHCTL_COMMAND_STATUS status = erase_flash(addr);

        if (status == DL_FLASHCTL_COMMAND_STATUS_FAILED) {
            printf("from %lu starting erase failed!\r\n", addr);
            protect_flash();
            return false;
        }
    }

    protect_flash();
    printf("starting erase succeed!\r\n");
    return true;
}
/**
xmodem 发送bin

*/
uint16_t xmodem_CRC16(uint8_t* data, uint16_t len) {
    uint16_t crc = 0x0000;
    while (len--) {
        crc ^= (*data++) << 8;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}
/**
 * @brief xmodem 接收 bin
 *
 * @return true
 * @return false
 */
bool xmodem_rec_bin() {
    if (boot0.flag & IAP_XMODE_FLAG) {
        if (boot0.time_out >= BOOT_TIME_OUT) {
            boot0.time_out = 0;
            printf("C");
        }
        if (u0.rx_size >= 133 && u0.rx_buf[0] == 0x01) {
            uint16_t calc = xmodem_CRC16(&u0.rx_buf[3], 128);
            uint16_t recv = (u0.rx_buf[131] << 8) | u0.rx_buf[132];

            if (calc == recv) {
                // CRC成功 → 保存到boot0.buf
                uint16_t offset = (boot0.xmodem_num % 8) * 128; // 8块=1KB
                memcpy(&boot0.buf[offset], &u0.rx_buf[3], 128);
                boot0.xmodem_num++;
                if ((boot0.xmodem_num % 8) == 0) {
                    download_new_app(A_START_ADDR, 8);
                }
                printf("%c", 0x06); // ACK
            } else {
                printf("%c", 0x15); // NAK
            }
            u0.rx_size = 0;
        }
        if (u0.rx_size == 1 && u0.rx_buf[0] == 0x04) {
            download_new_app(A_START_ADDR, boot0.xmodem_num % 8);
            printf("%c", 0x06);
            u0.rx_size = 0;
            return 1;
        }
    }
    return 0;
}
/**
 * @brief boot ordering enter
 *
 * @param xms
 * @return true
 * @return false
 */
bool boot_enter(uint16_t xms) {
    printf("%dms input 'o' enter ordering\r\n", xms);
    while (xms--) {
        delay_cycles(CPUCLK_FREQ / 1000);
        if (u0.rx_buf[0] == 'o') {
            boot0.flag = BOOT_ORDER_FLAG;
            return 1;
        }
    }
    return 0;
}
/**
 * @brief 直接跳转app程序
 *
 * @param app_start_addr
 * @return true
 * @return false 会跳转失败（原因bin flash failed）
 */
bool jump_to_app(uint32_t app_start_addr) {
    // 1. 定义函数指针类型（指向应用程序的复位向量）
    typedef void (*app_func_ptr)(void);
    // 2. 获取应用程序的栈指针（向量表第一个字）
    uint32_t app_stack = *(uint32_t*)app_start_addr;
    // 3. 获取应用程序的复位向量（向量表第二个字）
    uint32_t app_reset = *(uint32_t*)(app_start_addr + 4);

    // 4. 检查向量表是否有效（栈指针必须在SRAM范围内）
    if (app_stack >= SRAM_START_ADDR && app_stack <= SRAM_START_ADDR + SRAM_LEN * 1024) {

        // 5. 关闭所有中断
        NVIC_DisableIRQ(TIMER_0_INST_INT_IRQN);
        __disable_irq();

        // 6. 设置主栈指针（MSP）
        __set_MSP(app_stack);

        // 7. 跳转到应用程序的复位向量
        app_func_ptr app_entry = (app_func_ptr)app_reset;
        app_entry();
    }
    printf("jump failed\r\n");
    return 0;
}

bool download_new_app(uint16_t startAddr, uint8_t n) {
    uint32_t flash_data[2];
    uint16_t flash_addr = startAddr + boot0.offset;

    // 写入1KB (1024字节 = 128次 × 8字节)
    for (uint16_t i = 0; i < FLASH_ONE_PAGE_SIZE / 8 * n; i += 8) {
        // 组装8字节数据
        flash_data[0] = boot0.buf[i] | (boot0.buf[i + 1] << 8) | (boot0.buf[i + 2] << 16) | (boot0.buf[i + 3] << 24);

        flash_data[1] =
            boot0.buf[i + 4] | (boot0.buf[i + 5] << 8) | (boot0.buf[i + 6] << 16) | (boot0.buf[i + 7] << 24);

        // 写入Flash
        unprotect_flash();
        DL_FlashCTL_programMemory64WithECCGenerated(FLASHCTL, flash_addr + i, flash_data);
        protect_flash();
    }

    boot0.offset += FLASH_ONE_PAGE_SIZE;
    printf("."); // 进度点
    return true;
}
