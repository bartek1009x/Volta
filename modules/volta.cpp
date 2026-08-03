#include "volta.hpp"

extern bool SHOULD_QUIT;

int exit(lua_State* L) {
    SHOULD_QUIT = true;
    return 0;
}

int getVersion(lua_State* L) {
    lua_pushstring(L, "0.0.1");
    return 1;
}

void registerVoltaFunctions(ResourceState* state) {
    lua_State* L = state->getL();

    lua_pushcfunction(L, exit, "exit");
    lua_setfield(L, -2, "exit");
    lua_pushcfunction(L, getVersion, "getVersion");
    lua_setfield(L, -2, "getVersion");
}