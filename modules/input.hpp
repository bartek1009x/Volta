#ifndef INPUT_H
#define INPUT_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int isKeyDown(lua_State* L);
int isKeyUp(lua_State* L);
int isMouseButtonDown(lua_State* L);
int isMouseButtonUp(lua_State* L);
int getMousePosition(lua_State* L);
int getMouseWheelMove(lua_State* L);
void registerInputFunctions(lua_State* L, ResourceState* state);

#endif