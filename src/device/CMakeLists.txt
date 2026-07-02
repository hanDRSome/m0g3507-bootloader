# 收集当前目录下所有源文件
file(GLOB_RECURSE DEVICE_SRCS "*.c")

add_library(device STATIC ${DEVICE_SRCS})
target_include_directories(device PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Device 层依赖 BSP 层的硬件抽象接口
target_link_libraries(device PUBLIC bsp)
