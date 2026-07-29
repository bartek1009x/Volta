#include "system.hpp"

#include <cstdlib>

#include <SDL3/SDL_clipboard.h>

int getOS(lua_State *L) {
    lua_pushstring(L, SDL_GetPlatform());
    return 1;
}

int getEnv(lua_State *L) {
    lua_pushstring(L, std::getenv(lua_tostring(L, 1)));
    return 1;
}

int getClipboardText(lua_State *L) {
    lua_pushstring(L, SDL_GetClipboardText());
    return 1;
}

int setClipboardText(lua_State *L) {
    SDL_SetClipboardText(lua_tostring(L, 1));
    return 0;
}

void registerSystemFunctions(ResourceState* state) {
    lua_State* L = state->getL();

    lua_createtable(L, 1, 0);

    lua_pushcfunction(L, getOS, "getOS");
    lua_setfield(L, -2, "getOS");
    lua_pushcfunction(L, getEnv, "getEnv");
    lua_setfield(L, -2, "getEnv");
    lua_pushcfunction(L, getClipboardText, "getClipboardText");
    lua_setfield(L, -2, "getClipboardText");
    lua_pushcfunction(L, setClipboardText, "setClipboardText");
    lua_setfield(L, -2, "setClipboardText");

    lua_setglobal(L, "system");
}