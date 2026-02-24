cmake_minimum_required(VERSION 3.10)
project(bbd_demo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源文件
file(GLOB_RECURSE SOURCES "*.cc")

add_executable(bbd_demo ${SOURCES})

# 线程库
find_package(Threads REQUIRED)
target_link_libraries(bbd_demo PRIVATE Threads::Threads)

# 可执行文件输出到当前目录
set_target_properties(bbd_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
