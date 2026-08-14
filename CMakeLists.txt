cmake_minimum_required(VERSION 3.20)
project(volta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(dependencies/luau)
set(VOLTA_EXTRAS_DIR ${CMAKE_SOURCE_DIR}/volta_extras)

set(SOURCE_FILES
    main.cpp
    utils/Require.cpp
    utils/ResourceState.cpp
    utils/RequireContext.hpp
    utils/CoordinateTransformation.hpp
    utils/Pool.hpp
    modules/volta.cpp
    modules/window.cpp
    modules/graphics.cpp
    modules/input.cpp
    modules/audio.cpp
    modules/system.cpp
    modules/filesystem.cpp
    modules/network.cpp
    modules/color.cpp
    modules/sprite.cpp
    modules/animatedSprite.cpp
)

# SDL3
find_package(SDL3 QUIET CONFIG COMPONENTS SDL3-shared)

if (NOT SDL3_FOUND)
    message(STATUS "SDL3 not found locally; fetching SDL3")

    set(SDL_SHARED ON)
    set(SDL_STATIC OFF)
    set(SDL_TEST_LIBRARY OFF)
    set(SDL_TESTS OFF)
    set(SDL_EXAMPLES OFF)
    set(SDL_INSTALL OFF)

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.14
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL3)
endif()

# SDL3_image
find_package(SDL3_image QUIET CONFIG)

if (NOT SDL3_image_FOUND)
    message(STATUS "SDL3_image not found locally; fetching SDL3_image")

    set(BUILD_SHARED_LIBS ON)
    set(SDLIMAGE_VENDORED ON)
    set(SDLIMAGE_TESTS OFF)
    set(SDLIMAGE_SAMPLES OFF)
    set(SDLIMAGE_INSTALL OFF)

    FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.4.4
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL3_image)
endif()

# SDL3_mixer
find_package(SDL3_mixer QUIET CONFIG)

if (NOT SDL3_mixer_FOUND)
    message(STATUS "SDL3_mixer not found locally; fetching SDL3_mixer")

    set(BUILD_SHARED_LIBS ON)
    set(SDLMIXER_VENDORED ON)
    set(SDLMIXER_TESTS OFF)
    set(SDLMIXER_EXAMPLES OFF)
    set(SDLMIXER_INSTALL OFF)

    FetchContent_Declare(
        SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-3.2.4
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL3_mixer)
endif()

# SDL3_ttf
find_package(SDL3_ttf QUIET CONFIG)

if (NOT SDL3_ttf_FOUND)
    message(STATUS "SDL3_ttf not found locally; fetching SDL3_ttf")

    set(BUILD_SHARED_LIBS ON)
    set(SDLTTF_VENDORED ON)
    set(SDLTTF_TESTS OFF)
    set(SDLTTF_SAMPLES OFF)
    set(SDLTTF_INSTALL OFF)

    FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.2
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL3_ttf)
endif()

find_package(OpenSSL REQUIRED)
add_executable(volta ${SOURCE_FILES})

target_link_libraries(volta PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer SDL3_ttf::SDL3_ttf OpenSSL::SSL OpenSSL::Crypto Luau.Compiler Luau.Require Luau.VM)

if (WIN32)
    add_custom_command(TARGET volta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
            $<TARGET_FILE:OpenSSL::SSL>
            $<TARGET_FILE:OpenSSL::Crypto>
            $<TARGET_FILE_DIR:volta>
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:volta>/volta_extras
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${VOLTA_EXTRAS_DIR}
            $<TARGET_FILE_DIR:volta>/volta_extras
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
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
            $<TARGET_FILE:OpenSSL::SSL>
            $<TARGET_FILE:OpenSSL::Crypto>
            $<TARGET_FILE_DIR:volta>
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:volta>/volta_extras
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${VOLTA_EXTRAS_DIR}
            $<TARGET_FILE_DIR:volta>/volta_extras
    )
elseif (UNIX)
    set_target_properties(volta PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )

    set(APPDIR ${CMAKE_BINARY_DIR}/AppDir)

    add_custom_command(TARGET volta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:volta>/volta_extras
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${VOLTA_EXTRAS_DIR}
            $<TARGET_FILE_DIR:volta>/volta_extras
        COMMAND ${CMAKE_COMMAND} -E make_directory ${APPDIR}/usr/bin
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:volta> ${APPDIR}/usr/bin/volta
        COMMAND ${CMAKE_COMMAND} -E make_directory ${APPDIR}/usr/bin/extras
    )

    # AppImage requires a .desktop file and an icon
    file(WRITE ${CMAKE_BINARY_DIR}/volta.desktop
"[Desktop Entry]
Name=Volta
Exec=volta
Icon=volta
Type=Application
Categories=Game;
")

    find_program(LINUXDEPLOY linuxdeploy)
    find_program(APPIMAGETOOL appimagetool)

    if (LINUXDEPLOY)
        add_custom_target(appimage
            COMMAND ${CMAKE_COMMAND} -E env
                OUTPUT="volta.AppImage"
                NO_STRIP=1
                ${LINUXDEPLOY}
                    --appdir ${APPDIR}
                    --executable ${APPDIR}/usr/bin/volta
                    --desktop-file ${CMAKE_BINARY_DIR}/volta.desktop
                    --icon-file ${CMAKE_SOURCE_DIR}/assets/volta.png
                    --library $<TARGET_FILE:SDL3::SDL3>
                    --library $<TARGET_FILE:SDL3_image::SDL3_image>
                    --library $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
                    --library $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
                    --library $<TARGET_FILE:OpenSSL::SSL>
                    --library $<TARGET_FILE:OpenSSL::Crypto>
                    --output appimage
            DEPENDS volta
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    else()
        message(STATUS "linuxdeploy not found — 'appimage' target unavailable. "
                        "Download it from https://github.com/linuxdeploy/linuxdeploy/releases, "
                        "chmod +x, and put it on PATH.")
    endif()
endif()
