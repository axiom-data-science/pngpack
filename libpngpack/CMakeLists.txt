cmake_minimum_required(VERSION 3.13)
project(pngpack C)

set(CMAKE_C_STANDARD 11)

add_library(pngpack SHARED
        pngpack.c pngpack.h)

find_library(PNG_LIBRARY
        libpng.a
        PATHS "~/dev/opt/homebrew/opt/libpng/lib/"
        REQUIRED)
target_include_directories(pngpack PRIVATE "~/dev/opt/homebrew/opt/libpng/include/")
target_link_libraries(pngpack ${PNG_LIBRARY} z)

add_executable(example pngpack.c example.c)
target_include_directories(example PRIVATE "~/dev/opt/homebrew/opt/libpng/include/")
target_link_libraries(example ${PNG_LIBRARY} z)
