# GCC 实现bootloader 写入app程序

## 1.order 命令

- 'o' enter it
  - [0]reset
  - [1]update
  - [2]enter app

## 2.flash app '.bin' 二进制文件

- xmodem 协议 ，为啥用它，参考 bilibili.com/video/BV1SatHeBEVG/
- m0g3507 的专属（我测的最大）单次64b写入
- 可自行查库提升flash速度，但我认为已经够快了，主要是 发送方的速度需要提高

## 3.硬件配置

- uart: PA10(Tx),PA11(Rx)
- timer (随意): TIMA0
- oled 程序部分没加 ，可以自行DIY:PB2(SCL) PB3(SDA)

## 4.扩展

- uart 接 bluetooth 实现无线更新
- 默认 A区的地址，大小是 0x4000, 56 x 1024D

## 5.项目结构

```

- CMakeLists.txt
- src
  - app
  - device(可不用)
  - bsp
- syscfg
  - ti_msp_dl_config.h
  - ti_msp_dl_config.c
  - mspm0g3507.lds
  - (PROJECTROOT).syscfg
- tools
  - (有线flash+配置文件)
- (me)

```

## 6.克隆问题

- 测试电脑要有
  - arm-none-eabi
  - cmake
  - cortex-Debug
  - ti-mspm0_sdk

## 7.日志

- 2026.7.4 更新xmodem 的1KB 快速下载；修复了一些特定名词的bug; 减少了函数的继承的纰漏
