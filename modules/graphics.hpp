#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../ResourceState.hpp"

int setDrawColor(lua_State* L);
int clear(lua_State* L);
int loadImage(lua_State* L);
int drawImage(lua_State* L);
void registerGraphicsFunctions(ResourceState* state);

#endif