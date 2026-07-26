cmake_minimum_required(VERSION 3.20)
project(volta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(dependencies/luau)

set(SOURCE_FILES
    main.cpp
    utils/require.cpp
    utils/ResourceState.cpp
    utils/Pool.hpp
    modules/window.cpp
    modules/graphics.cpp
    modules/input.cpp
    modules/audio.cpp
    modules/system.cpp
)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_mixer REQUIRED CONFIG)
add_executable(volta ${SOURCE_FILES})

target_link_libraries(volta PRIVATE SDL3::SDL3 SDL3_image SDL3_mixer Luau.Compiler Luau.VM)

if (WIN32)
    add_custom_command(TARGET volta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
            $<TARGET_FILE_DIR:volta>
    )
elseif (APPLE)
    set_target_properties(volta PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
    add_custom_command(TARGET volta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
            $<TARGET_FILE_DIR:volta>
    )
elseif (UNIX)
    set_target_properties(volta PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )

    add_custom_command(TARGET volta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
            $<TARGET_FILE_DIR:volta>

        COMMAND ${CMAKE_COMMAND} -E create_symlink
            $<TARGET_FILE_NAME:SDL3::SDL3>
            $<TARGET_FILE_DIR:volta>/libSDL3.so.0

        COMMAND ${CMAKE_COMMAND} -E create_symlink
            $<TARGET_FILE_NAME:SDL3_image::SDL3_image>
            $<TARGET_FILE_DIR:volta>/libSDL3_image.so.0

        COMMAND ${CMAKE_COMMAND} -E create_symlink
            $<TARGET_FILE_NAME:SDL3_mixer::SDL3_mixer>
            $<TARGET_FILE_DIR:volta>/libSDL3_mixer.so.0
    )
endif()