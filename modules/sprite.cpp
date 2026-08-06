#include "sprite.hpp"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <string>
#include <unordered_map>

#include "../dependencies/luau/VM/include/lualib.h"
#include "graphics.hpp"

static SDL_Renderer *renderer = nullptr;

int spriteNew(lua_State *L) {
    int x = luaL_optnumber(L, 1, 0);
    int y = luaL_optnumber(L, 2, 0);
    int w = luaL_optnumber(L, 3, 10);
    int h = luaL_optnumber(L, 4, 10);

    int textureId;

    if (lua_isnumber(L, 5)) {
        textureId = lua_tointeger(L, 5);
    } else if (lua_isstring(L, 5)) {
        textureId = loadImagePath(L, lua_tostring(L, 5));
    }

    int r = luaL_optinteger(L, 6, 0);

    lua_newtable(L);

    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, w);
    lua_setfield(L, -2, "w");

    lua_pushnumber(L, h);
    lua_setfield(L, -2, "h");

    lua_pushinteger(L, textureId);
    lua_setfield(L, -2, "textureId");

    lua_pushinteger(L, r);
    lua_setfield(L, -2, "r");

    lua_pushboolean(L, false);
    lua_setfield(L, -2, "flipH");

    lua_pushboolean(L, false);
    lua_setfield(L, -2, "flipV");

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);

    return 1;
}

static SDL_FRect SPRITE_RECT;

int draw(lua_State *L) {
    // self at index 1

    lua_rawgetfield(L, 1, "x");
    lua_rawgetfield(L, 1, "y");
    lua_rawgetfield(L, 1, "w");
    lua_rawgetfield(L, 1, "h");
    lua_rawgetfield(L, 1, "r");
    lua_rawgetfield(L, 1, "textureId");
    lua_rawgetfield(L, 1, "flipH");
    lua_rawgetfield(L, 1, "flipV");

    float x = lua_tonumber(L, -8);
    float y = lua_tonumber(L, -7);
    float w = lua_tonumber(L, -6);
    float h = lua_tonumber(L, -5);
    int r = lua_tointeger(L, -4);
    int textureId = lua_tointeger(L, -3);
    bool flipH = lua_toboolean(L, -2);
    bool flipV = lua_toboolean(L, -1);

    SPRITE_RECT.x = x;
    SPRITE_RECT.y = y;
    SPRITE_RECT.w = w;
    SPRITE_RECT.h = h;
    if (r == 0 && !flipH && !flipV) {
        SDL_RenderTexture(renderer, getTextureById(textureId), nullptr, &SPRITE_RECT);
    } else {
        SDL_FlipMode flip;
        if (flipH && flipV) {
            flip = SDL_FLIP_HORIZONTAL_AND_VERTICAL;
        } else if (flipH) {
            flip = SDL_FLIP_HORIZONTAL;
        } else if (flipV) {
            flip = SDL_FLIP_VERTICAL;
        } else {
            flip = SDL_FLIP_NONE;
        }
        SDL_RenderTextureRotated(renderer, getTextureById(textureId), nullptr, &SPRITE_RECT, r, nullptr, flip);
    }

    return 0;
}

int setTexture(lua_State *L) {
    // self at index 1

    lua_pushinteger(L, loadImagePath(L, lua_tostring(L, 2)));
    lua_setfield(L, -3, "textureId");

    return 0;
}

int setTextureId(lua_State *L) {
    // self at index 1

    lua_pushinteger(L, lua_tointeger(L, 2));
    lua_setfield(L, -3, "textureId");

    return 0;
}

int setPositionS(lua_State *L) {
    // self at index 1

    lua_pushinteger(L, lua_tonumber(L, 2));
    lua_setfield(L, -3, "x");
    lua_pushinteger(L, lua_tonumber(L, 3));
    lua_setfield(L, -3, "y");

    return 0;
}

int setSizeS(lua_State *L) {
    // self at index 1

    lua_pushinteger(L, lua_tonumber(L, 2));
    lua_setfield(L, -3, "w");
    lua_pushinteger(L, lua_tonumber(L, 3));
    lua_setfield(L, -3, "h");

    return 0;
}

int setRotation(lua_State *L) {
    // self at index 1

    lua_pushinteger(L, lua_tointeger(L, 2));
    lua_setfield(L, -3, "r");

    return 0;
}

int flipHorizontal(lua_State *L) {
    // self at index 1

    lua_pushboolean(L, lua_toboolean(L, 2));
    lua_setfield(L, -3, "flipH");

    return 0;
}

int flipVertical(lua_State *L) {
    // self at index 1

    lua_pushboolean(L, lua_toboolean(L, 2));
    lua_setfield(L, -3, "flipV");

    return 0;
}

int getTextureId(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "textureId");
    return 1;
}

int getPositionS(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "x");
    lua_rawgetfield(L, 1, "y");
    return 2;
}

int getSizeS(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "w");
    lua_rawgetfield(L, 1, "h");
    return 2;
}

int getRotation(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "r");
    return 1;
}

int getFlipHorizontal(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "flipH");
    return 1;
}

int getFlipVertical(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "flipV");
    return 1;
}

void pushSpriteClass(lua_State *L) {
    lua_newtable(L);

    lua_pushvalue(L, -1);

    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushvalue(L, -1);
    lua_pushcclosure(L, spriteNew, "spriteNew", 1);
    lua_setfield(L, -2, "new");

    lua_pushcfunction(L, draw, "draw");
    lua_setfield(L, -2, "draw");

    lua_pushcfunction(L, setTexture, "setTexture");
    lua_setfield(L, -2, "setTexture");

    lua_pushcfunction(L, setTextureId, "setTextureId");
    lua_setfield(L, -2, "setTextureId");

    lua_pushcfunction(L, setPositionS, "setPosition");
    lua_setfield(L, -2, "setPosition");

    lua_pushcfunction(L, setSizeS, "setSize");
    lua_setfield(L, -2, "setSize");

    lua_pushcfunction(L, setRotation, "setRotation");
    lua_setfield(L, -2, "setRotation");

    lua_pushcfunction(L, flipHorizontal, "flipHorizontal");
    lua_setfield(L, -2, "flipHorizontal");

    lua_pushcfunction(L, flipVertical, "flipVertical");
    lua_setfield(L, -2, "flipVertical");

    lua_pushcfunction(L, getTextureId, "getTextureId");
    lua_setfield(L, -2, "getTextureId");

    lua_pushcfunction(L, getPositionS, "getPosition");
    lua_setfield(L, -2, "getPosition");

    lua_pushcfunction(L, getSizeS, "getSize");
    lua_setfield(L, -2, "getSize");

    lua_pushcfunction(L, getRotation, "getRotation");
    lua_setfield(L, -2, "getRotation");

    lua_pushcfunction(L, getFlipHorizontal, "getFlipHorizontal");
    lua_setfield(L, -2, "getFlipHorizontal");

    lua_pushcfunction(L, getFlipVertical, "getFlipVertical");
    lua_setfield(L, -2, "getFlipVertical");

    // class table remains on stack
}

void registerSpriteObject(ResourceState* state) {
    if (renderer == nullptr) {
        renderer = state->getRenderer();
    }
    lua_State* L = state->getL();

    pushSpriteClass(L);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "Sprite");
}