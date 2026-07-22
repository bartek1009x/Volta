cmake_minimum_required(VERSION 3.20)
project(volta)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(dependencies/luau)

set(SOURCE_FILES main.cpp require.cpp)
add_executable(volta ${SOURCE_FILES})

target_link_libraries(volta PRIVATE Luau.Compiler Luau.VM)