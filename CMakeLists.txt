CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # 新增编译命令导出
# 增加调试信息
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
project(snake-c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加包含目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

ADD_SUBDIRECTORY(src snake-c)
