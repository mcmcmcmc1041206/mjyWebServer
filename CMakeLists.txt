#设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(webserver)

#生成debug版本
set(CMAKE_BUILD_TYPE "DEBUG")

#设置项目可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置项目编译头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)

add_subdirectory(src)
add_subdirectory(test)