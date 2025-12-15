cmake_minimum_required(VERSION 3.10)
project(CAN_RECEIVER)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 查找线程库
find_package(Threads REQUIRED)

add_executable(CAN_RECEIVER main.cpp can_receiver.cpp)
target_compile_options(CAN_RECEIVER PRIVATE -Wall -O2)

# 链接线程库
target_link_libraries(CAN_RECEIVER PRIVATE Threads::Threads)
