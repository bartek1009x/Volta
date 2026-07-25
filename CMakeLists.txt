cmake_minimum_required(VERSION 3.20)
project(volta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(dependencies/luau)

set(SOURCE_FILES main.cpp utils/require.cpp utils/ResourceState.cpp utils/Pool.hpp modules/window.cpp modules/graphics.cpp modules/input.cpp modules/audio.cpp modules/system.cpp)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG REQUIRED COMPONENTS)
find_package(SDL3_mixer REQUIRED CONFIG REQUIRED COMPONENTS)
add_executable(volta ${SOURCE_FILES})

target_link_libraries(volta PRIVATE SDL3::SDL3 SDL3_image SDL3_mixer Luau.Compiler Luau.VM)