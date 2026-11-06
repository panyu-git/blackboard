#!/bin/bash
set -e
BUILD_DIR=build

# 仓库里提交的 CMake 缓存写死了作者机器上的源码路径，换目录后 cmake 会直接失败。
if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
	cached_src=$(sed -n 's/^CMAKE_HOME_DIRECTORY:INTERNAL=//p' "$BUILD_DIR/CMakeCache.txt" | head -n 1)
	if [ -n "$cached_src" ] && [ "$cached_src" != "$(pwd)" ]; then
		echo "Removing CMake cache created at $cached_src"
		rm -rf "$BUILD_DIR"
	fi
fi

# 创建构建目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 运行 CMake 和 make
cmake ..
make -j$(nproc)

# 可执行文件已输出到上级目录
cd ..
echo "Build finished. Executable: ./bbd_demo"
