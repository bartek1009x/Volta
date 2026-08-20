#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL_render.h>

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"
#include "../utils/CoordinateTransformation.hpp"

extern TransformationStack transformStack;
extern Transform currentTransform;

SDL_Texture* getTextureById(int id);
int setCursorVisibility(lua_State* L);
int isCursorVisible(lua_State* L);
int setVsync(lua_State* L);
int isVsyncEnabled(lua_State* L);
int setDrawColor(lua_State* L);
int clear(lua_State* L);
int drawRect(lua_State* L);
int drawCircle(lua_State* L);
int loadImagePath(lua_State* L, const char* path);
int loadImage(lua_State* L);
int setTextureScaleMode(lua_State* L);
int unloadImage(lua_State* L);
int drawImage(lua_State* L);
int drawImageRegion(lua_State* L);
int loadFont(lua_State* L);
int unloadFont(lua_State* L);
int drawText(lua_State* L);
int pushCoord(lua_State* L);
int translateCoord(lua_State* L);
int scaleCoord(lua_State* L);
int rotateCoord(lua_State* L);
int shearCoord(lua_State* L);
int popCoord(lua_State* L);
int setScissor(lua_State* L);
int getScissor(lua_State* L);
void registerGraphicsFunctions(lua_State* L, ResourceState* state);
void updateFontTextCache();

#endif
