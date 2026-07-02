#pragma once
#include "my_bsp_config.h"
/**
 * @brief  syscfg 中配置GPIO GROUP == MPU6050_PORT
 *                                  MY_SDA MY_SCL
 */
#ifdef MPU6050_PORT
// 打开SDA引脚（输出）
static inline void SDA_OUT(void) {
    DL_GPIO_initDigitalOutput(MPU6050_MY_SDA_IOMUX);
    gpio_set(MPU6050_PORT, MPU6050_MY_SDA_PIN);
    DL_GPIO_enableOutput(MPU6050_PORT, MPU6050_MY_SDA_PIN);
}
// 关闭SDA引脚（输入）
static inline void SDA_IN(void) {

    DL_GPIO_initDigitalInputFeatures(MPU6050_MY_SDA_IOMUX, DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
                                     DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}

/*引脚配置层*/

static inline void MyI2C_W_SCL(uint8_t BitValue) {
    if (BitValue)
        gpio_set(MPU6050_PORT, MPU6050_MY_SCL_PIN);
    else
        gpio_clear(MPU6050_PORT, MPU6050_MY_SCL_PIN);
    delay_us(8);
}

static inline void MyI2C_W_SDA(uint8_t BitValue) {
    SDA_OUT();
    if (BitValue)
        gpio_set(MPU6050_PORT, MPU6050_MY_SDA_PIN);
    else
        gpio_clear(MPU6050_PORT, MPU6050_MY_SDA_PIN);
    delay_us(8);
}

static inline uint8_t MyI2C_R_SDA(void) {
    uint8_t b;
    uint32_t BitValue;
    SDA_IN();
    BitValue = gpio_read(MPU6050_PORT, MPU6050_MY_SDA_PIN); // 读取SDA电平
    {
        if (BitValue)
            b = 1;
        else
            b = 0;
    }
    delay_us(8); // 延时8us，防止时序频率超过要求
    return b;    // 返回SDA电平
}
#define MYI2C_READY
void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_R_SDA(void);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);
#define MPU6050_READY
#endif

#ifdef OLED_PORT
#define FONT_8X16 8
#define OLED_CMD 0  // 写命令
#define OLED_DATA 1 // 写数据
#ifndef GPIO_OLED_PIN_OLED_SCL_PORT
#define GPIO_OLED_PIN_OLED_SCL_PORT OLED_PORT
#endif

#ifndef GPIO_OLED_PIN_OLED_SDA_PORT
#define GPIO_OLED_PIN_OLED_SDA_PORT OLED_PORT
#endif

#ifndef IIC_delay
#define IIC_delay() delay_cycles((CPUCLK_FREQ / 1000 / 1000) * 10)
#endif
#ifndef IIC_delay300ms
#define IIC_delay300ms() delay_cycles((CPUCLK_FREQ / 1000) * 300)
#endif

// OLED SSD1306 I2C 时钟SCL
#define OLED_SCL_Set() (DL_GPIO_setPins(GPIO_OLED_PIN_OLED_SCL_PORT, OLED_OLED_SCL_PIN))
#define OLED_SCL_Clr() (DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_SCL_PORT, OLED_OLED_SCL_PIN))

// OLED SSD1306 I2C 数据SDA
#define OLED_SDA_Set() (DL_GPIO_setPins(GPIO_OLED_PIN_OLED_SDA_PORT, OLED_OLED_SDA_PIN))
#define OLED_SDA_Clr() (DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_SDA_PORT, OLED_OLED_SDA_PIN))
#define OLED_SDA_Read() DL_GPIO_readPins(GPIO_OLED_PIN_OLED_SDA_PORT, OLED_OLED_SDA_PIN)
#define OLED_READY
#endif