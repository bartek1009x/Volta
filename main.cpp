#include <iostream>
#include <fstream>
#include <filesystem>

#include "dependencies/luau/VM/include/lua.h"
#include "dependencies/luau/VM/include/lualib.h"
#include "dependencies/luau/Compiler/include/luacode.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#include "ResourceState.hpp"
#include "require.cpp"
#include "modules/window.hpp"
#include "modules/graphics.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    // luau
    ResourceState state{};

    lua_State* L = state.getL();

    lua_pushcfunction(L, luau_require, "require");
    lua_setglobal(L, "require");

    filesystem::path scriptPath = filesystem::current_path() / argv[1];
    state.setMainPath(scriptPath.parent_path());
    auto size = filesystem::file_size(scriptPath);
    string script(size, '\0');
    ifstream in{scriptPath};
    in.read(&script[0], size);

    size_t bytecode_size;
    char* bytecode = luau_compile(script.c_str(), script.length(), nullptr, &bytecode_size);
    string chunkname = "=" + scriptPath.string();
    int res = luau_load(L, chunkname.c_str(), bytecode, bytecode_size, 0);
    free(bytecode);

    if (res != 0) {
        size_t len;
        const char* msg = lua_tolstring(L, -1, &len);
        lua_pop(L, 1);
        printf("Failed to compile: %s\n", msg);
        return 1;
    }

    // register modules
    registerWindowFunctions(&state);
    registerGraphicsFunctions(&state);

    // luau init
    lua_State* T = lua_newthread(L);
    lua_pushvalue(L, -2);
    lua_remove(L, -3);
    lua_xmove(L, T, 1);

    int status = lua_resume(T, nullptr, 0);
    if (status != LUA_OK) {
        const char* err = lua_tostring(T, -1);
        printf("Runtime error: %s\n", err ? err : "unknown error");
    }

    lua_getglobal(L, "init");

    if (lua_pcall(L, 0, 1, 0) != 0) {
        printf("Runtime error: %s\n", lua_tostring(L, -1));
        return 1;
    }

    // sdl3
    SDL_Event event;
    bool closeWindow = false;

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    SDL_Renderer* renderer = state.getRenderer();
    while (!closeWindow) {
        SDL_PollEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            closeWindow = true;
        }

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double) (NOW - LAST) / (double )SDL_GetPerformanceFrequency();

        // call luau update function
        lua_getglobal(L, "update");
        lua_pushnumber(L, deltaTime);

        if (lua_pcall(L, 1, 1, 0) != 0) {
            printf("Runtime error: %s\n", lua_tostring(L, -1));
        }
        lua_pop(L, 1);

        SDL_RenderPresent(renderer);
    }

    return 0;
}