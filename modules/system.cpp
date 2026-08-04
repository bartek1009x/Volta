#include "system.hpp"

#include <cstdlib>

#include <SDL3/SDL_clipboard.h>

#include "../dependencies/luau/VM/include/lualib.h"

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

static const luaL_Reg system_lib[] = {
    {"getOS", getOS},
    {"getEnv", getEnv},
    {"getClipboardText", getClipboardText},
    {"setClipboardText", setClipboardText},
    {nullptr, nullptr},
};

void registerSystemFunctions(ResourceState* state) {
    lua_State* L = state->getL();

    luaL_register(L, "system", system_lib);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "system");
}