#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int animatedSpriteNew(lua_State *L);
int fromSpritesheet(lua_State *L);
int drawA(lua_State *L);
int setPositionA(lua_State* L);
int setSizeA(lua_State* L);
int setRotationA(lua_State* L);
int setTextureIds(lua_State* L);
int setSpritesheet(lua_State* L);
int flipHorizontalA(lua_State* L);
int flipVerticalA(lua_State* L);
int getTextureIdA(lua_State* L);
int getCurrentFrame(lua_State* L);
int getRegions(lua_State* L);
int getPositionA(lua_State* L);
int getSizeA(lua_State* L);
int getRotationA(lua_State* L);
int getFlipHorizontalA(lua_State* L);
int getFlipVerticalA(lua_State* L);
void pushASpriteClass(lua_State* L);
void registerAnimatedSpriteObject(ResourceState* state);

#endif
