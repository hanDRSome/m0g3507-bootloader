#include "bsp_i2c_soft.h"

#ifdef MYI2C_READY
/**
 * 函    数：I2C初始化
 * 参    数：无
 * 返 回 值：无
 * 注意事项：此函数需要用户实现内容，实现SCL和SDA引脚的初始化
 */
void MyI2C_Init(void) {
    SYSCFG_DL_GPIO_init();
    /*设置默认电平*/
    DL_GPIO_setPins(MPU6050_PORT,
                    MPU6050_MY_SDA_PIN | MPU6050_MY_SCL_PIN); // 设置PA8和PA9引脚初始化后默认为高电平（释放总线状态）
}

/*协议层*/

/**
 * 函    数：I2C起始
 * 参    数：无
 * 返 回 值：无
 */
void MyI2C_Start(void) {
    SDA_OUT();
    MyI2C_W_SDA(1); // 释放SDA，确保SDA为高电平
    MyI2C_W_SCL(1); // 释放SCL，确保SCL为高电平
    MyI2C_W_SDA(0); // 在SCL高电平期间，拉低SDA，产生起始信号
    MyI2C_W_SCL(0); // 起始后拉低SCL，为了占用总线，方便总线时序的拼接
}

/**
 * 函    数：I2C终止
 * 参    数：无
 * 返 回 值：无
 */
void MyI2C_Stop(void) {
    SDA_OUT();
    MyI2C_W_SDA(0); // 拉低SDA，确保SDA为低电平
    MyI2C_W_SCL(1); // 释放SCL，使SCL呈现高电平
    MyI2C_W_SDA(1); // 在SCL高电平期间，释放SDA，产生终止信号
}

/**
 * 函    数：I2C发送一个字节
 * 参    数：Byte 要发送的一个字节数据，范围：0x00~0xFF
 * 返 回 值：无
 */
void MyI2C_SendByte(uint8_t Byte) {
    SDA_OUT();
    uint8_t i;
    for (i = 0; i < 8; i++) // 循环8次，主机依次发送数据的每一位
    {
        MyI2C_W_SDA(Byte & (0x80 >> i)); // 使用掩码的方式取出Byte的指定一位数据并写入到SDA线
        MyI2C_W_SCL(1);                  // 释放SCL，从机在SCL高电平期间读取SDA
        MyI2C_W_SCL(0);                  // 拉低SCL，主机开始发送下一位数据
    }
}

/**
 * 函    数：I2C接收一个字节
 * 参    数：无
 * 返 回 值：接收到的一个字节数据，范围：0x00~0xFF
 */
uint8_t MyI2C_ReceiveByte(void) {
    SDA_OUT();
    uint8_t i, Byte = 0x00; // 定义接收的数据，并赋初值0x00
    MyI2C_W_SDA(1);         // 接收前，主机先确保释放SDA，避免干扰从机的数据发送
    for (i = 0; i < 8; i++) // 循环8次，主机依次接收数据的每一位
    {
        SDA_IN();
        MyI2C_W_SCL(1); // 释放SCL，主机机在SCL高电平期间读取SDA
        if (MyI2C_R_SDA() == 1) {
            Byte |= (0x80 >> i);
        } // 读取SDA数据，并存储到Byte变量
        // 当SDA为1时，置变量指定位为1，当SDA为0时，不做处理，指定位为默认的初值0
        MyI2C_W_SCL(0); // 拉低SCL，从机在SCL低电平期间写入SDA
    }
    return Byte; // 返回接收到的一个字节数据
}

/**
 * 函    数：I2C发送应答位
 * 参    数：Byte 要发送的应答位，范围：0~1，0表示应答，1表示非应答
 * 返 回 值：无
 */
void MyI2C_SendAck(uint8_t AckBit) {
    SDA_OUT();
    MyI2C_W_SDA(AckBit); // 主机把应答位数据放到SDA线
    MyI2C_W_SCL(1);      // 释放SCL，从机在SCL高电平期间，读取应答位
    MyI2C_W_SCL(0);      // 拉低SCL，开始下一个时序模块
}

/**
 * 函    数：I2C接收应答位
 * 参    数：无
 * 返 回 值：接收到的应答位，范围：0~1，0表示应答，1表示非应答
 */
uint8_t MyI2C_ReceiveAck(void) {
    SDA_OUT();
    uint8_t AckBit; // 定义应答位变量
    MyI2C_W_SDA(1); // 接收前，主机先确保释放SDA，避免干扰从机的数据发送
    MyI2C_W_SCL(1); // 释放SCL，主机机在SCL高电平期间读取SDA
    SDA_IN();
    AckBit = MyI2C_R_SDA(); // 将应答位存储到变量里
    MyI2C_W_SCL(0);         // 拉低SCL，开始下一个时序模块
    return AckBit;          // 返回定义应答位变量
}

#endif

