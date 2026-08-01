#include "volta.hpp"

extern bool SHOULD_QUIT;

int exit(lua_State* L) {
    SHOULD_QUIT = true;
    return 0;
}

void registerVoltaFunctions(ResourceState* state) {
    lua_State* L = state->getL();

    lua_pushcfunction(L, exit, "exit");
    lua_setfield(L, -2, "exit");
}