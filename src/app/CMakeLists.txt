# 应用层源文件（main.c 等）
file(GLOB APP_SRCS "*.c")

# SysConfig 自动生成的驱动配置代码+MSPM0 启动文件（向量表、Reset_Handler 等）
file(GLOB SYSCFG_SRCS "${PROJECT_SOURCE_DIR}/syscfg/*.c")


# 链接为 ELF 可执行文件
add_executable(${PROJECT_NAME}.elf
    ${APP_SRCS}
    ${SYSCFG_SRCS}
    ${STARTUP_SRC}
    ${FREERTOS_SRCS}
)

# 输出到 build/out/
set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out"
)

# 头文件搜索路径
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FREERTOS_INCLUDES}
)

# 链接 BSP 和 Device 静态库
target_link_libraries(${PROJECT_NAME}.elf PRIVATE bsp device m)

# 链接脚本与 GCC 参数
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T "${PROJECT_SOURCE_DIR}/syscfg/mspm0g3507.lds"    # 链接脚本：定义内存布局与段映射 
    -mcpu=cortex-m0plus                                 # 目标 CPU：Cortex-M0+（仅 Thumb 指令集）
    -mthumb                                             # 使用 Thumb 指令集（Cortex-M 必须）
    -specs=nano.specs                                   # 使用 newlib-nano：精简版 C 标准库（减小固件体积）
    -Wl,-u,_write                                       # 强制链接 bsp_uart.o 中的 _write（否则静态库不会自动拉入）
    -specs=nosys.specs                                  # 系统调用桩（_close/_read 等），_write 由 bspuart.c 覆盖
    -Wl,--gc-sections                                   # 链接器垃圾回收：移除未引用的代码段和数据段
    -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}.elf>/${PROJECT_NAME}.map  # 生成 .map 文件（用于分析内存占用）
)

# ── 编译后处理：生成 .hex / .bin / .out 烧录文件 ──────────────────
set(FLASH_SIZE "0x5000") # 引导程序的大小（只针对hex）

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
        --gap-fill=0xFF
        $<TARGET_FILE:${PROJECT_NAME}.elf>
        $<TARGET_FILE_DIR:${PROJECT_NAME}.elf>/${PROJECT_NAME}.hex #hex的每个地址都有数据
    COMMAND ${CMAKE_OBJCOPY} -O binary
        $<TARGET_FILE:${PROJECT_NAME}.elf>
        $<TARGET_FILE_DIR:${PROJECT_NAME}.elf>/${PROJECT_NAME}.bin
    COMMENT "Generating aligned .hex file..."
)