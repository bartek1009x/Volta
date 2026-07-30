# Volta
Volta is a WIP 2D game development framework for Luau written in C++. This is a passion project and one I'll use to learn more about C++, so expect the code to not be of the highest quality. Will try to use AI as sparingly as possible for the code itself, to learn as much myself as possible.

# Usage
You can run your game by running your luau script through the Volta executable like this: `volta main.luau` (where the filename can be anything other than main too)

# Building and dependencies
To build the framework yourself you need SDL3, SDL3_image, SDL3_mixer and SDL3_ttf installed on your system and Luau cloned into dependencies/luau (I'm currently developing the framework with Luau commit 5bc7f4b). 

## Linux
For building an AppImage on Linux, linuxdeploy is needed. You can build an AppImage with `cmake --build build --target appimage`. A regular `cmake --build build` will build an executable that **won't** have SDL3 and other dependencies bundled and it's overall not recommended.
