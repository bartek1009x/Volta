#include "window.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

SDL_Window* window;

int setTitle(lua_State* L) {
    SDL_SetWindowTitle(window, lua_tostring(L, 1));
    return 0;
}

int setFullscreen(lua_State* L) {
    SDL_SetWindowFullscreen(window, lua_toboolean(L, 1));
    return 0;
}

int setBorderless(lua_State* L) {
    SDL_SetWindowBordered(window, !lua_toboolean(L, 1));
    return 0;
}

int setResizable(lua_State* L) {
    SDL_SetWindowResizable(window, lua_toboolean(L, 1));
    return 0;
}

int setPosition(lua_State* L) {
    SDL_SetWindowPosition(window, lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}

int setSize(lua_State* L) {
    SDL_SetWindowSize(window, lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}

int setMaxSize(lua_State* L) {
    SDL_SetWindowMaximumSize(window, lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}

int setMinSize(lua_State* L) {
    SDL_SetWindowMinimumSize(window, lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}

int hasFocus(lua_State* L) {
    lua_pushboolean(L, SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS);
    return 1;
}

int getTitle(lua_State* L) {
    lua_pushstring(L, SDL_GetWindowTitle(window));
    return 1;
}

int isFullscreen(lua_State* L) {
    lua_pushboolean(L, SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
    return 1;
}

int isBorderless(lua_State* L) {
    lua_pushboolean(L, SDL_GetWindowFlags(window) & SDL_WINDOW_BORDERLESS);
    return 1;
}

int isResizable(lua_State* L) {
    lua_pushboolean(L, SDL_GetWindowFlags(window) & SDL_WINDOW_RESIZABLE);
    return 1;
}

int getPosition(lua_State* L) {
    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

int getSize(lua_State* L) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}

int getMinSize(lua_State* L) {
    int w, h;
    SDL_GetWindowMinimumSize(window, &w, &h);
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
}

int getMaxSize(lua_State* L) {
    int w, h;
    SDL_GetWindowMaximumSize(window, &w, &h);
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
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
    lua_pushcfunction(L, hasFocus, "hasFocus");
    lua_setfield(L, -2, "hasFocus");
    lua_pushcfunction(L, getTitle, "getTitle");
    lua_setfield(L, -2, "getTitle");
    lua_pushcfunction(L, isFullscreen, "isFullscreen");
    lua_setfield(L, -2, "isFullscreen");
    lua_pushcfunction(L, isBorderless, "isBorderless");
    lua_setfield(L, -2, "isBorderless");
    lua_pushcfunction(L, isResizable, "isResizable");
    lua_setfield(L, -2, "isResizable");
    lua_pushcfunction(L, getPosition, "getPosition");
    lua_setfield(L, -2, "getPosition");
    lua_pushcfunction(L, getSize, "getSize");
    lua_setfield(L, -2, "getSize");
    lua_pushcfunction(L, getMinSize, "getMinSize");
    lua_setfield(L, -2, "getMinSize");
    lua_pushcfunction(L, getMaxSize, "getMaxSize");
    lua_setfield(L, -2, "getMaxSize");

    lua_setreadonly(L, -1, 1);

    lua_setfield(L, -2, "window");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    }

    SDL_Renderer* renderer;
    if (!SDL_CreateWindowAndRenderer("Volta", 320, 240, 0, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
    }

    state->setWinRen(window, renderer);
}