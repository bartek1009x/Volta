cmake_minimum_required(VERSION 3.20)
project(volta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

add_subdirectory(dependencies/box2d)
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
    modules/box2d.cpp
)

if(MINGW)
    set(WIN_LIBS "${CMAKE_SOURCE_DIR}/dependencies/win-libs")

    set(OPENSSL_ROOT_DIR
        "${WIN_LIBS}/sources/openssl-3.6.3/x86_64-w64-mingw32"
        CACHE PATH "OpenSSL root directory"
    )

    set(OPENSSL_USE_STATIC_LIBS TRUE)

    list(APPEND CMAKE_PREFIX_PATH
        "${WIN_LIBS}/SDL3-3.4.12/x86_64-w64-mingw32"
        "${WIN_LIBS}/SDL3_image-3.4.4/x86_64-w64-mingw32"
        "${WIN_LIBS}/SDL3_mixer-3.2.4/x86_64-w64-mingw32"
        "${WIN_LIBS}/SDL3_ttf-3.2.2/x86_64-w64-mingw32"
        "${OPENSSL_ROOT_DIR}"
    )
endif()

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

# ------------------------------------------------------------
# Prebuilt DirectXShaderCompiler
# ------------------------------------------------------------

if(MINGW)
    set(DXC_ROOT
        "${CMAKE_SOURCE_DIR}/dependencies/SDL_shadercross/external/DirectXShaderCompiler-binaries"
    )

    if(NOT EXISTS "${DXC_ROOT}/windows/inc/dxcapi.h")
        message(STATUS "Prebuilt DXC not found; downloading it")

        include(
            "${CMAKE_SOURCE_DIR}/dependencies/SDL_shadercross/build-scripts/download-prebuilt-DirectXShaderCompiler.cmake"
        )
    endif()

    # Used later when copying the runtime DLLs next to volta.exe.
    set(DXC_BIN_DIR "${DXC_ROOT}/windows/bin/x64")
endif()

# ------------------------------------------------------------
# SPIRV-Cross
# ------------------------------------------------------------

set(SPIRV_CROSS_SHARED OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_STATIC ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_SKIP_INSTALL ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_FORCE_PIC ON CACHE BOOL "" FORCE)

add_subdirectory(
    dependencies/SDL_shadercross/external/SPIRV-Cross
    EXCLUDE_FROM_ALL
)

add_library(spirv_cross_c ALIAS spirv-cross-c)


# ------------------------------------------------------------
# SDL_shadercross
# ------------------------------------------------------------

set(SDLSHADERCROSS_VENDORED OFF CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_SHARED ON CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_STATIC OFF CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_SPIRVCROSS_SHARED OFF CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_DXC ON CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_CLI OFF CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_TESTS OFF CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(dependencies/SDL_shadercross)


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

add_custom_command(TARGET volta POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/volta.luau
        $<TARGET_FILE_DIR:volta>/volta.luau
)

target_link_libraries(volta PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer SDL3_ttf::SDL3_ttf SDL3_shadercross::SDL3_shadercross OpenSSL::SSL OpenSSL::Crypto Luau.Compiler Luau.Require Luau.VM Luau.CodeGen box2d)

if(WIN32)
    add_custom_command(TARGET volta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf>

            $<TARGET_FILE:SDL3_shadercross::SDL3_shadercross>

            "${DXC_BIN_DIR}/dxcompiler.dll"
            "${DXC_BIN_DIR}/dxil.dll"

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

            $<TARGET_FILE:SDL3_shadercross::SDL3_shadercross>

            $<TARGET_FILE:DirectXShaderCompiler::dxcompiler>
            $<TARGET_FILE:DirectXShaderCompiler::dxil>

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

    file(WRITE ${CMAKE_BINARY_DIR}/volta.desktop
"[Desktop Entry]
Name=Volta
Exec=volta
Icon=volta
Type=Application
Categories=Game;
")

    find_program(LINUXDEPLOY
        NAMES linuxdeploy linuxdeploy-x86_64.AppImage
    )

    find_program(APPIMAGETOOL
        NAMES appimagetool appimagetool-x86_64.AppImage
    )

    if (NOT LINUXDEPLOY)
        message(WARNING
            "linuxdeploy not found. The 'appimage' target will fail until "
            "linuxdeploy is available on PATH."
        )
    endif()

    if (NOT APPIMAGETOOL)
        message(WARNING
            "appimagetool not found. The 'appimage' target will fail until "
            "appimagetool is available on PATH."
        )
    endif()

    add_custom_target(appimage
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${APPDIR}

        COMMAND ${CMAKE_COMMAND} -E make_directory
            ${APPDIR}/usr/bin

        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:volta>
            ${APPDIR}/usr/bin/volta

        COMMAND ${CMAKE_COMMAND} -E make_directory
            ${APPDIR}/usr/bin/volta_extras

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${VOLTA_EXTRAS_DIR}
            ${APPDIR}/usr/bin/volta_extras

        COMMAND ${CMAKE_COMMAND} -E env
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
                --library $<TARGET_FILE:SDL3_shadercross::SDL3_shadercross>
                --library $<TARGET_FILE:DirectXShaderCompiler::dxcompiler>
                --library $<TARGET_FILE:OpenSSL::SSL>
                --library $<TARGET_FILE:OpenSSL::Crypto>

        COMMAND ${CMAKE_STRIP}
            --strip-unneeded
            ${APPDIR}/usr/lib/libdxcompiler.so

        COMMAND ${CMAKE_STRIP}
            --strip-unneeded
            ${APPDIR}/usr/lib/libSDL3_shadercross.so.0

        COMMAND ${CMAKE_COMMAND} -E env
            ARCH=x86_64
            ${APPIMAGETOOL}
            ${APPDIR}
            ${CMAKE_BINARY_DIR}/volta.AppImage

        DEPENDS volta

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        VERBATIM
    )
endif()