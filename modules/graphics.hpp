#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL_render.h>

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

SDL_Texture* getTextureById(int id);
int setCursorVisibility(lua_State* L);
int setVsync(lua_State* L);
int setDrawColor(lua_State* L);
int clear(lua_State* L);
int drawRect(lua_State* L);
int drawCircle(lua_State* L);
int loadImagePath(lua_State* L, const char* path);
int loadImage(lua_State* L);
int setTextureScaleMode(lua_State* L);
int unloadImage(lua_State* L);
int drawImage(lua_State* L);
int loadFont(lua_State* L);
int unloadFont(lua_State* L);
int drawText(lua_State* L);
void registerGraphicsFunctions(ResourceState* state);
void updateFontTextCache();

#endif
