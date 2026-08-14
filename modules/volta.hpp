#ifndef VOLTA_H
#define VOLTA_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int exit(lua_State* L);
int getDelta(lua_State* L);
int getVersion(lua_State* L);
void registerVoltaFunctions(ResourceState* state);

#endif