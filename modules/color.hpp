#ifndef COLOR_H
#define COLOR_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int colorNew(lua_State *L);
int fromHex(lua_State *L);
int fromHexa(lua_State *L);
int toHex(lua_State *L);
int toHexa(lua_State *L);
int lerp(lua_State *L);
void pushClass(lua_State* L);
void registerColorObject(ResourceState* state);

#endif