#ifndef WINDOW_H
#define WINDOW_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int setTitle(lua_State* L);
int setWindowIcon(lua_State* L);
int setFullscreen(lua_State* L);
int setBorderless(lua_State* L);
int setResizable(lua_State* L);
int setPosition(lua_State* L);
int setSize(lua_State* L);
int setMinSize(lua_State* L);
int setMaxSize(lua_State* L);
int hasFocus(lua_State* L);
int getTitle(lua_State* L);
int isFullscreen(lua_State* L);
int isBorderless(lua_State* L);
int isResizable(lua_State* L);
int getPosition(lua_State* L);
int getSize(lua_State* L);
int getMinSize(lua_State* L);
int getMaxSize(lua_State* L);
void registerWindowFunctions(ResourceState* state);

#endif