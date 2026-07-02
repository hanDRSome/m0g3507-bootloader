# 收集当前目录下所有源文件
file(GLOB_RECURSE BSP_SRCS "*.c")

# 创建静态库
add_library(bsp STATIC ${BSP_SRCS})

# 暴露头文件路径给引用它的模块
target_include_directories(bsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# BSP 需要使用 TI SDK 的底层驱动库 
target_link_libraries(bsp PRIVATE ${SDK_PATH}/source/ti/driverlib/lib/gcc/m0p/mspm0g1x0x_g3x0x/driverlib.a)
