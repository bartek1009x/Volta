# Volta
Volta is a WIP 2D game development framework for Luau written in C++. This is a passion project and one I'll use to learn more about C++, so expect the code to not be of the highest quality.

It is theoretically cross platform, but currently only officially supports Windows, Linux and macOS (though **macOS has no official builds**, so you will have to compile the framework yourself for it). Android and iOS support is planned in the future.

# Usage
You can run your game by running your luau script through the Volta executable like this: `volta main.luau` (where the filename can be anything other than main too)

# AI usage in the project
Used AI to write code sparingly - not to vibecode too much, just to assist with some tasks, explain some things, find bugs, etc.
The only parts of the project that used AI extensively were:
- the CMake configuration for building,
- the `require()` implementation,
- the Animated Sprite object.

Besides that only a little AI code here and there, sparingly.

# Building and dependencies
To build the framework yourself you need SDL3, SDL3_image, SDL3_mixer, SDL3_ttf and OpenSSL installed on your system.
Additionally, Luau has to be cloned into `dependencies/luau` (I'm currently developing the framework with Luau commit 5bc7f4b).
`httplib.h` also has to be put into `dependencies/cpp-httplib` (currently using the header from release v0.52.0).

Specific OS notes are listed below.

## Linux
For building an AppImage on Linux, linuxdeploy is needed. You can build an AppImage with `cmake --build build --target appimage`. A regular `cmake --build build` will build an executable that **won't** have SDL3 and other dependencies bundled and it's overall not recommended.

## Windows
On Windows the SDL libraries are put as `.dll`s alongside the volta executable instead of everything being in a single executable like it is with AppImage on Linux. 

For cross compiling to Windows from Linux, there's a special cmake toolchain for Windows specifically in `cmake/toolchains/mingw-w64-x86_64.cmake`. Also, `SDL3-3.4.12`, `SDL3_image-3.4.4`, `SDL3_mixer-3.2.4` and `SDL3_ttf-3.2.2` must be present in `dependencies/win-libs`. These should be downloaded from the official SDL GitHub repositories (eg. Releases -> `SDL3-devel-3.4.12-mingw.zip`).

## macOS
I don't know man I don't even have a Mac. I think you should be able to build volta fine though?
