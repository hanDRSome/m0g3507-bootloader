#pragma once
#include "bsp_uart.h"
#include "my_bsp_config.h"

// flash 相关的宏
#define unprotect_flash() DL_FlashCTL_unprotectMainMemory(FLASHCTL)
#define protect_flash() DL_FlashCTL_protectMainMemory(FLASHCTL)
#define erase_flash(addr) DL_FlashCTL_eraseMemoryFromRAM(FLASHCTL, addr, DL_FLASHCTL_COMMAND_SIZE_SECTOR)
#define write_flash_8byte(flash_addr, flash_data)                                                                      \
    DL_FlashCTL_programMemory64WithECCGenerated(FLASHCTL, flash_addr, flash_data)
// B区与A区地址宏
#define FLASH_START_ADDR (0x00000000)
#define SRAM_START_ADDR (0x20200000)
#define SRAM_LEN (0x00008000)
#define PAGE_NUM (128)                   // flash的总长
#define FLASH_ONE_PAGE_SIZE (0x00000400) // 1KB扇区大小
#define B_PAGE (16)
#define A_PAGE ((PAGE_NUM - B_PAGE) / 2) // 56KB
#define A_START_ADDR (FLASH_START_ADDR + B_PAGE * 1024)

#define BOOT_TIME_OUT (1000)
#define BOOT_ORDER_FLAG (0x01)
// xmoderm
#define IAP_XMODEMD_SIZE (128)
#define IAP_XMODEM_FLAG (0x02)
#define IAP_XMODEMD_END_FLAG (0x04)
// 应答
#define SOH 0x01 // 128字节数据块
#define STX 0x02 // 1024字节数据块 (Ymodem-1K)
#define EOT 0x04 // 结束
#define ACK 0x06 // 确认
#define NAK 0x15 // 否定确认
#define CAN 0x18 // 取消
#define CRC 0x43 // 'C'

typedef struct {
    uint8_t buf[FLASH_ONE_PAGE_SIZE];
    uint16_t offset; // 0 ~ 1024*56
    uint8_t flag;
    int32_t modem_num;
    uint32_t time_out;
} boot_t;

extern boot_t boot0;
__attribute__((section(".ramfunc"))) bool erase_flash_region(uint32_t startAddr, uint32_t size);
uint16_t xmodem_CRC16(uint8_t* data, uint16_t len);
bool boot_enter(uint16_t xms);
bool jump_to_app(uint32_t app_start_addr);
bool download_new_app(uint16_t startAddr, uint8_t n);
bool xmodem_128B_rec_bin();
bool xmodem_1k_rec_bin();