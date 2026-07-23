#include "window.hpp"
#include <SDL3/SDL_video.h>

SDL_Window* window;
SDL_Renderer *renderer;

int setTitle(lua_State* L) {
    SDL_SetWindowTitle(window, lua_tostring(L, 1));
    return 1;
}

int setFullscreen(lua_State* L) {
    SDL_SetWindowFullscreen(window, lua_toboolean(L, 1));
    return 1;
}

int setBorderless(lua_State* L) {
    SDL_SetWindowBordered(window, !lua_toboolean(L, 1));
    return 1;
}

int setResizable(lua_State* L) {
    SDL_SetWindowResizable(window, lua_toboolean(L, 1));
    return 1;
}

int setPosition(lua_State* L) {
    SDL_SetWindowPosition(window, lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 1;
}

int setSize(lua_State* L) {
    SDL_SetWindowSize(window, lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 1;
}

int setMaxSize(lua_State* L) {
    SDL_SetWindowMaximumSize(window, lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 1;
}

int setMinSize(lua_State* L) {
    SDL_SetWindowMinimumSize(window, lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 1;
}

void registerWindowFunctions(ResourceState* state) {
    lua_State* L = state->getL();

    lua_createtable(L, 1, 0);

    lua_pushcfunction(L, setTitle, "setTitle");
    lua_setfield(L, -2, "setTitle");
    lua_pushcfunction(L, setFullscreen, "setFullscreen");
    lua_setfield(L, -2, "setFullscreen");
    lua_pushcfunction(L, setBorderless, "setBorderless");
    lua_setfield(L, -2, "setBorderless");
    lua_pushcfunction(L, setResizable, "setResizable");
    lua_setfield(L, -2, "setResizable");
    lua_pushcfunction(L, setPosition, "setPosition");
    lua_setfield(L, -2, "setPosition");
    lua_pushcfunction(L, setSize, "setSize");
    lua_setfield(L, -2, "setSize");
    lua_pushcfunction(L, setMinSize, "setMinSize");
    lua_setfield(L, -2, "setMinSize");
    lua_pushcfunction(L, setMaxSize, "setMaxSize");
    lua_setfield(L, -2, "setMaxSize");

    lua_setglobal(L, "window");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer("Volta", 320, 240, 0, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
    }

    state->setWinRen(window, renderer);
}