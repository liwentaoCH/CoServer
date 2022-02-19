cmake_minimum_required(VERSION 2.8)
project(sylar)

set(CMAKE_VERBOSE_MAKEFILE ON)
# -Wno  关闭警告      -Wno-deprecated 告诉编译器 g++ 使用 C++ 标准废弃的特性不告警
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(/usr/local/include)
# 指定将要生成的libsylar与系统libmysql路径
link_directories(/disk2/WebServer/sylar/lib 
                 /usr/lib64/mysql) 

set(LIB_SRC
    src/log.cc
    src/config.cc
    src/env.cc
    src/thread.cc
    src/util.cc
    src/mutex.cc
    src/fiber.cc
    src/scheduler.cc
    src/iomanager.cc
    src/timer.cc
    src/hook.cc
    src/fd_manager.cc
    src/address.cc
    src/socket.cc
    src/bytearray.cc
    src/http/http.cc
    src/http/http11_parser.cc
    src/http/httpclient_parser.cc
    src/http/http_parser.cc
    src/tcp_server.cc
    src/stream.cc
    src/streams/socket_stream.cc
    src/http/http_session.cc
    src/http/http_server.cc
    src/http/servlet.cc
    src/uri.cc
    src/mysql_pool.cc
    )

add_library(sylar SHARED ${LIB_SRC})

set(LIB_LIB
    dl
    sylar
    pthread
    yaml-cpp
    mysqlclient
    )
    
add_executable(http_server /disk2/WebServer/sylar/example/http_server.cc)
add_dependencies(http_server sylar)        #提醒test可执行文件的生成 需要sylar.so中的符号定义，所以先生存sylar.so
target_link_libraries(http_server ${LIB_LIB})

# 指定生成的可执行文件路径
SET(EXECUTABLE_OUTPUT_PATH /disk2/WebServer/sylar/bin)
# 指定生成的动态链接文件路径
SET(LIBRARY_OUTPUT_PATH /disk2/WebServer/sylar/lib)
