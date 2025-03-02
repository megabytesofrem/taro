cmake_minimum_required(VERSION 3.10)
project(taro C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unused-variable")

# Include source files
include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SRCS
    src/runtime/*.c
    src/util/*.c
    src/*.c
)

add_executable(taro ${SRCS})

add_custom_target(clean_all
    COMMENT "Cleaning up build artifacts"
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/obj
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/taro
)
