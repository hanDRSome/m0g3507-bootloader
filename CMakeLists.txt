# 目标系统：ARM 裸机
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 交叉编译器
set(CMAKE_C_COMPILER "D:/Env/msys2/ucrt64/bin/arm-none-eabi-gcc.exe")

# 编译器检查时只编译不链接，避免缺少启动文件/链接脚本导致失败
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 防止交叉编译时误用宿主系统库
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
cmake_minimum_required(VERSION 3.15)

# 1. 定义项目名称和语言
project(demo C ASM)

set(CMAKE_C_STANDARD 99)

# 2. 定位 TI MSPM0 SDK_PATH 
set(SDK_PATH "D:/ti/mspm0_sdk")

# 3. 全局编译选项与宏定义
add_compile_options(
    -mcpu=cortex-m0plus
    -mthumb
    -ffunction-sections
    -fdata-sections
    -Wall
    -O0
)
add_compile_definitions(__MSPM0G3507__)

# 4. 全局包含 SDK 头文件路径
include_directories(
    ${SDK_PATH}/source
    ${SDK_PATH}/source/third_party/CMSIS/Core/Include
    ${PROJECT_SOURCE_DIR}/syscfg
)

# 5. 添加各个子模块目录
add_subdirectory(src/bsp)
add_subdirectory(src/device)
add_subdirectory(src/app)
