project(server)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_C_COMPILER g++)
set(CMAKE_BUILD_TYPE "Debug")
add_compile_options(-std=c++11)
set(C_FLAGS
    -g
)

add_subdirectory(myServer)