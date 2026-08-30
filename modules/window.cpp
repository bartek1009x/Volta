#include "window.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#include "../dependencies/luau/VM/include/lualib.h"

SDL_Window* window;
static ResourceState* resourceState = nullptr;

int setTitle(lua_State* L) {
    SDL_SetWindowTitle(window, lua_tostring(L, 1));
    return 0;
}

int setWindowIcon(lua_State* L) {
    const char* path = lua_tostring(L, 1);
    const std::filesystem::path finalPath = resourceState->getMainPath() / path;
    const std::string stringPath = finalPath.string(); // we need to convert it here, because if we don't, path.c_str() will return const wchar_t* on windows (we need const char*)
    const char* pathCStr = stringPath.c_str();
    SDL_Surface* icon = IMG_Load(pathCStr);
    if (icon == nullptr) {
        luaL_error(L, "Could not load image: %s", pathCStr);
        return 0;
    }

    SDL_SetWindowIcon(window, icon);

    SDL_DestroySurface(icon);

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

static const luaL_Reg window_lib[] = {
    {"setTitle", setTitle},
    {"setWindowIcon", setWindowIcon},
    {"setFullscreen", setFullscreen},
    {"setBorderless", setBorderless},
    {"setResizable", setResizable},
    {"setPosition", setPosition},
    {"setSize", setSize},
    {"setMinSize", setMinSize},
    {"setMaxSize", setMaxSize},
    {"hasFocus", hasFocus},
    {"getTitle", getTitle},
    {"isFullscreen", isFullscreen},
    {"isBorderless", isBorderless},
    {"isResizable", isResizable},
    {"getPosition", getPosition},
    {"getSize", getSize},
    {"getMinSize", getMinSize},
    {"getMaxSize", getMaxSize},
    {nullptr, nullptr},
};

void registerWindowFunctions(lua_State* L, ResourceState* state) {
    resourceState = state;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    }

    window = SDL_CreateWindow("Volta", 320, 240, 0);
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
    }

    state->setWindow(window);

    luaL_register(L, "window", window_lib);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "window");
}