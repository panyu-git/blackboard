#!/bin/bash
set -e
BUILD_DIR=build

# 创建构建目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 运行 CMake 和 make
cmake ..
make -j$(nproc)

# 可执行文件已输出到上级目录
cd ..
echo "Build finished. Executable: ./bbd_demo"
