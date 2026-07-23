#ifndef WINDOW_H
#define WINDOW_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../ResourceState.hpp"

int setTitle(lua_State* L);
int setFullscreen(lua_State* L);
int setBorderless(lua_State* L);
int setResizable(lua_State* L);
int setPosition(lua_State* L);
int setSize(lua_State* L);
int setMinSize(lua_State* L);
int setMaxSize(lua_State* L);
void registerWindowFunctions(ResourceState* state);

#endif