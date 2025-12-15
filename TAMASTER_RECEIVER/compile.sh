#!/bin/bash
rm -r bin
rm -r build
# 项目根目录
PROJECT_DIR=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="${PROJECT_DIR}/build"
BIN_DIR="${PROJECT_DIR}/bin"

# 检查交叉编译器是否存在
if ! command -v aarch64-linux-gcc &> /dev/null; then
    echo "错误: 找不到交叉编译器 aarch64-linux-gcc"
    echo "请确保交叉编译器在 PATH 环境变量中"
    exit 1
fi


# 创建构建目录
echo "创建构建目录..."
mkdir -p "$BUILD_DIR"
mkdir -p "$BIN_DIR"

# 进入构建目录
cd "$BUILD_DIR" || exit 1

# 运行 CMake 配置
echo "运行 CMake 配置..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchain-aarch64.cmake \
    -DCMAKE_BUILD_TYPE=Release

# 检查 CMake 配置是否成功
if [ $? -ne 0 ]; then
    echo "CMake 配置失败!"
    exit 1
fi

# 编译项目
echo "开始编译..."
make -j$(nproc)

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "编译成功!" 
    echo "可执行文件: $BIN_DIR/CAN_RECEIVER"
else
    echo "编译失败!"
    exit 1
fi
