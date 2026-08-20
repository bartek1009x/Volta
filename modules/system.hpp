#ifndef SYSTEM_H
#define SYSTEM_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int getOS(lua_State* L);
int getEnv(lua_State* L);
int getClipboardText(lua_State* L);
int setClipboardText(lua_State* L);
void registerSystemFunctions(lua_State* L, ResourceState* state);

#endif