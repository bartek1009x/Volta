#include "volta.hpp"

#include "../dependencies/luau/VM/include/lualib.h"

extern bool SHOULD_QUIT;

int exit(lua_State* L) {
    SHOULD_QUIT = true;
    return 0;
}

extern time_t startTimestamp;
int getPlaytime(lua_State* L) {
    time_t now;
    time(&now);
    lua_pushnumber(L, now - startTimestamp);
    return 1;
}

extern double deltaTime;
int getDelta(lua_State* L) {
    lua_pushnumber(L, deltaTime);
    return 1;
}

int getVersion(lua_State* L) {
    lua_pushstring(L, "0.2.0");
    return 1;
}

static const luaL_Reg volta_lib[] = {
    {"exit", exit},
    {"getPlaytime", getPlaytime},
    {"getVersion", getVersion},
    {"getDelta", getDelta},
    {nullptr, nullptr},
};

void registerVoltaFunctions(lua_State* L, ResourceState* state) {
    luaL_register(L, nullptr, volta_lib);
}