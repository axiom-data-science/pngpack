cmake_minimum_required(VERSION 3.13)
project(pngpack C)

set(CMAKE_C_STANDARD 11)

add_executable(example main.c)

target_include_directories(example PRIVATE "${CMAKE_SOURCE_DIR}/../")
target_link_directories(example PRIVATE "${CMAKE_SOURCE_DIR}/../cmake-build-release/")
target_link_libraries(example libpngpack.dylib)
