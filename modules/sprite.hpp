#ifndef SPRITE_H
#define SPRITE_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int spriteNew(lua_State *L);
int draw(lua_State *L);
int setTexture(lua_State* L);
int setTextureId(lua_State* L);
int setPositionS(lua_State* L);
int setSizeS(lua_State* L);
int setRotation(lua_State* L);
int flipHorizontal(lua_State* L);
int flipVertical(lua_State* L);
int getTextureId(lua_State* L);
int getPositionS(lua_State* L);
int getSizeS(lua_State* L);
int getRotation(lua_State* L);
int getFlipHorizontal(lua_State* L);
int getFlipVertical(lua_State* L);
void pushSpriteClass(lua_State* L);
void registerSpriteObject(ResourceState* state);

#endif