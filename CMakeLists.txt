cmake_minimum_required(VERSION 3.20)
project(volta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(dependencies/luau)
set(VOLTA_EXTRAS_DIR ${CMAKE_SOURCE_DIR}/volta_extras)

set(SOURCE_FILES
    main.cpp
    utils/ExecutablePath.hpp
    utils/require.cpp
    utils/ResourceState.cpp
    utils/Pool.hpp
    modules/window.cpp
    modules/graphics.cpp
    modules/input.cpp
    modules/audio.cpp
    modules/system.cpp
    modules/filesystem.cpp
    modules/color.cpp
)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_mixer REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
add_executable(volta ${SOURCE_FILES})

target_link_libraries(volta PRIVATE SDL3::SDL3 SDL3_image SDL3_mixer SDL3_ttf Luau.Compiler Luau.VM)

if (WIN32)
    add_custom_command(TARGET volta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
            $<TARGET_FILE_DIR:volta>
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:volta>/extras
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${VOLTA_EXTRAS_DIR}
            $<TARGET_FILE_DIR:volta>/extras
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
            $<TARGET_FILE_DIR:volta>
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:volta>/extras
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${VOLTA_EXTRAS_DIR}
            $<TARGET_FILE_DIR:volta>/extras
    )
elseif (UNIX)
    set_target_properties(volta PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )

    set(APPDIR ${CMAKE_BINARY_DIR}/AppDir)

    add_custom_command(TARGET volta POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${APPDIR}/usr/bin
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:volta> ${APPDIR}/usr/bin/volta
        COMMAND ${CMAKE_COMMAND} -E make_directory ${APPDIR}/usr/bin/extras
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${VOLTA_EXTRAS_DIR} ${APPDIR}/usr/bin/extras
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
                ${LINUXDEPLOY}
                    --appdir ${APPDIR}
                    --executable ${APPDIR}/usr/bin/volta
                    --desktop-file ${CMAKE_BINARY_DIR}/volta.desktop
                    --icon-file ${CMAKE_SOURCE_DIR}/assets/volta.png
                    --library $<TARGET_FILE:SDL3::SDL3>
                    --library $<TARGET_FILE:SDL3_image::SDL3_image>
                    --library $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
                    --library $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
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