# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.17)
# 项目信息
project (TinyRenderer)
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)
# 添加 common 子目录
add_subdirectory(common)
# 指定生成目标
add_executable(TinyRenderer main.cpp)
# 添加链接库
target_link_libraries(TinyRenderer Com)