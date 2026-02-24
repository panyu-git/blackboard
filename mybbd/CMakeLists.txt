cmake_minimum_required(VERSION 3.10)
project(bbd_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源文件（排除 examples 目录）
file(GLOB_RECURSE ALL_SOURCES "${CMAKE_SOURCE_DIR}/*.cc")
set(SOURCES)
foreach(_f IN LISTS ALL_SOURCES)
	if(NOT _f MATCHES ".*/examples/.*")
		list(APPEND SOURCES ${_f})
	else()
		message(STATUS "Skipping example source: ${_f}")
	endif()
endforeach()

add_executable(bbd_demo ${SOURCES})

# 线程库
find_package(Threads REQUIRED)
target_link_libraries(bbd_demo PRIVATE Threads::Threads)

# 可执行文件输出到当前目录
set_target_properties(bbd_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
