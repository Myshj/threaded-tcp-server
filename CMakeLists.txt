cmake_minimum_required(VERSION 3.14)
project(threaded-tcp-server C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

add_executable(threaded-tcp-server main.c logging.h server.h server.c)

if(WIN32)
    target_link_libraries(threaded-tcp-server wsock32 ws2_32)
endif()