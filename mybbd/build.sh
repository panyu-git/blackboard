#!/bin/bash
set -e
BUILD_DIR=__build

# 如果第一个参数是 "test"，选择编译 examples 下的示例
if [ "$1" = "test" ]; then
	TEST_NUM="$2"
	case "$TEST_NUM" in
		01)
			echo "Building example t01_function_traits..."
			g++ -std=c++17 -I. examples/t01_function_traits.cc -o examples/t01_function_traits
			echo "Built: examples/t01_function_traits";
			;;
		02)
			echo "Building example t02_check_traits..."
			g++ -std=c++17 -I. examples/t02_check_traits.cc -o examples/t02_check_traits
			echo "Built: examples/t02_check_traits";
			;;
		03)
			echo "Building example t03_variadic_templates..."
			g++ -std=c++17 -I. examples/t03_variadic_templates.cc -o examples/t03_variadic_templates
			echo "Built: examples/t03_variadic_templates";
			;;
        04)
            echo "Building example t04_perfect_forwarding..."
            g++ -std=c++17 -I. examples/t04_perfect_forwarding.cc -o examples/t04_perfect_forwarding
            echo "Built: examples/t04_perfect_forwarding";
            ;;
		05)
            echo "Building example t05_variadic_template..."
            g++ -std=c++17 -I. examples/t05_variadic_template.cc -o examples/t05_variadic_template
            echo "Built: examples/t05_variadic_template";
            ;;
		07)
			echo "Building example t07_tuple_get_impl..."
			g++ -std=c++17 -I. examples/t07_tuple_get_impl.cc -o examples/t07_tuple_get_impl
			echo "Built: examples/t07_tuple_get_impl";
			;;
		*)
			echo "Usage: $0 test <01|02|03|04|05|07>";
			exit 1;
			;;
	esac
	exit 0
fi

# 默认行为：使用 CMake 构建主工程并生成 bbd_demo
# 创建构建目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 运行 CMake 和 make
cmake ..
make -j $(nproc)

# 可执行文件已输出到上级目录
cd ..
echo "Build finished. Executable: ./bbd_demo"
