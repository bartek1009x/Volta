#include "volta.hpp"

#include "../dependencies/luau/VM/include/lualib.h"

extern bool SHOULD_QUIT;

int exit(lua_State* L) {
    SHOULD_QUIT = true;
    return 0;
}

extern double deltaTime;
int getDelta(lua_State* L) {
    lua_pushnumber(L, deltaTime);
    return 1;
}

int getVersion(lua_State* L) {
    lua_pushstring(L, "0.0.8");
    return 1;
}

static const luaL_Reg volta_lib[] = {
    {"exit", exit},
    {"getVersion", getVersion},
    {"getDelta", getDelta},
    {nullptr, nullptr},
};

void registerVoltaFunctions(ResourceState* state) {
    lua_State* L = state->getL();

    luaL_register(L, nullptr, volta_lib);
}