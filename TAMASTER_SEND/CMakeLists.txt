cmake_minimum_required(VERSION 3.10)
project(CAN_SEND)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 查找线程库
find_package(Threads REQUIRED)

add_executable(CAN_SEND main.cpp)
target_compile_options(CAN_SEND PRIVATE -Wall -O2)

# 链接线程库
target_link_libraries(CAN_SEND PRIVATE Threads::Threads)
