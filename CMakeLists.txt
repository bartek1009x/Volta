cmake_minimum_required(VERSION 3.20)
project(volta)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(dependencies/luau)

set(SOURCE_FILES main.cpp require.cpp ResourceState.cpp modules/window.cpp)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
add_executable(volta ${SOURCE_FILES})

target_link_libraries(volta PRIVATE SDL3::SDL3 Luau.Compiler Luau.VM)