#include "animatedSprite.hpp"

#include <cmath>

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "../dependencies/luau/VM/include/lualib.h"
#include "graphics.hpp"

static SDL_Renderer *renderer = nullptr;

extern SDL_FPoint transformPoint(float x, float y);
extern SDL_FPoint transformSpritePoint(float px, float py, float spriteX, float spriteY, float pivotX, float pivotY, float radians);

int animatedSpriteNew(lua_State *L) {
    int x = luaL_optnumber(L, 1, 0);
    int y = luaL_optnumber(L, 2, 0);
    int w = luaL_optnumber(L, 3, 10);
    int h = luaL_optnumber(L, 4, 10);
    double frameDuration = lua_tonumber(L, 5);

    int firstFrameArgument = 6;
    int argumentCount = lua_gettop(L);

    if (!std::isfinite(frameDuration) || frameDuration <= 0.0) {
        luaL_argerror(L, 5, "frameDuration must be a positive number");
        return 0;
    }

    if (argumentCount < firstFrameArgument) {
        luaL_error(L, "AnimatedSprite.new requires at least one frame texture ID");
        return 0;
    }

    int frameCount = argumentCount - firstFrameArgument + 1;

    lua_newtable(L);
    int spriteIndex = lua_gettop(L);

    lua_pushnumber(L, x);
    lua_setfield(L, spriteIndex, "x");

    lua_pushnumber(L, y);
    lua_setfield(L, spriteIndex, "y");

    lua_pushnumber(L, w);
    lua_setfield(L, spriteIndex, "w");

    lua_pushnumber(L, h);
    lua_setfield(L, spriteIndex, "h");

    lua_pushnumber(L, frameDuration);
    lua_setfield(L, spriteIndex, "frameDuration");

    lua_pushnumber(L, 0.0);
    lua_setfield(L, spriteIndex, "elapsedTime");

    lua_pushinteger(L, 1);
    lua_setfield(L, spriteIndex, "currentFrame");

    lua_pushinteger(L, frameCount);
    lua_setfield(L, spriteIndex, "frameCount");

    for (int argument = firstFrameArgument; argument <= argumentCount; ++argument) {
        int textureId = lua_tointeger(L, argument);
        int frameIndex = argument - firstFrameArgument + 1;

        lua_pushinteger(L, textureId);
        lua_rawseti(L, spriteIndex, frameIndex);
    }

    lua_pushinteger(L, 0);
    lua_setfield(L, spriteIndex, "r");

    lua_pushboolean(L, false);
    lua_setfield(L, spriteIndex, "flipH");

    lua_pushboolean(L, false);
    lua_setfield(L, spriteIndex, "flipV");

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, spriteIndex);

    return 1;
}

int fromSpritesheet(lua_State* L) {
    double x = luaL_optnumber(L, 1, 0);
    double y = luaL_optnumber(L, 2, 0);
    double w = luaL_optnumber(L, 3, 10);
    double h = luaL_optnumber(L, 4, 10);

    lua_rawgetfield(L, 5, "textureId");
    int textureId = lua_tonumber(L, -1);

    double frameDuration = luaL_checknumber(L, 6);

    if (!std::isfinite(frameDuration) || frameDuration <= 0.0) {
        luaL_argerror(L, 6, "frameDuration must be a positive number");
        return 0;
    }

    lua_rawgetfield(L, 5, "regions");
    int regionsIndex = lua_gettop(L);

    int frameCount = (int) (lua_objlen(L, regionsIndex));

    if (frameCount <= 0) {
        luaL_argerror(L, 5, "Spritesheet must contain at least one region");
        return 0;
    }

    lua_newtable(L);
    int spriteIndex = lua_gettop(L);

    lua_pushnumber(L, x);
    lua_setfield(L, spriteIndex, "x");

    lua_pushnumber(L, y);
    lua_setfield(L, spriteIndex, "y");

    lua_pushnumber(L, w);
    lua_setfield(L, spriteIndex, "w");

    lua_pushnumber(L, h);
    lua_setfield(L, spriteIndex, "h");

    lua_pushnumber(L, frameDuration);
    lua_setfield(L, spriteIndex, "frameDuration");

    lua_pushnumber(L, 0.0);
    lua_setfield(L, spriteIndex, "elapsedTime");

    lua_pushinteger(L, 1);
    lua_setfield(L, spriteIndex, "currentFrame");

    lua_pushinteger(L, frameCount);
    lua_setfield(L, spriteIndex, "frameCount");

    lua_pushinteger(L, textureId);
    lua_setfield(L, spriteIndex, "textureId");

    lua_pushvalue(L, regionsIndex);
    lua_setfield(L, spriteIndex, "regions");

    lua_pushinteger(L, 0);
    lua_setfield(L, spriteIndex, "r");

    lua_pushboolean(L, false);
    lua_setfield(L, spriteIndex, "flipH");

    lua_pushboolean(L, false);
    lua_setfield(L, spriteIndex, "flipV");

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, spriteIndex);

    return 1;
}

int drawA(lua_State* L) {
    // self at index 1
    double deltaTime = luaL_checknumber(L, 2);

    lua_rawgetfield(L, 1, "frameDuration");
    double frameDuration = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_rawgetfield(L, 1, "elapsedTime");
    double elapsedTime = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_rawgetfield(L, 1, "currentFrame");
    int currentFrame = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_rawgetfield(L, 1, "frameCount");
    int frameCount = lua_tointeger(L, -1);
    lua_pop(L, 1);

    if (frameCount <= 0) {
        luaL_error(L, "AnimatedSprite has no frames");
        return 0;
    }

    if (currentFrame < 1 || currentFrame > frameCount) {
        currentFrame = 1;
    }

    elapsedTime += deltaTime;

    if (elapsedTime >= frameDuration) {
        double completedFrames = std::floor(elapsedTime / frameDuration);

        int framesToAdvance = (int) (std::fmod(completedFrames, (double) frameCount));

        currentFrame = ((currentFrame - 1 + framesToAdvance) % frameCount) + 1;

        elapsedTime = std::fmod(elapsedTime, frameDuration);
    }

    lua_pushnumber(L, elapsedTime);
    lua_setfield(L, 1, "elapsedTime");

    lua_pushinteger(L, currentFrame);
    lua_setfield(L, 1, "currentFrame");

    int textureId;
    SDL_FRect sourceRect{};
    SDL_FRect* srcRectPointer = nullptr;

    lua_rawgetfield(L, 1, "regions");

    if (lua_istable(L, -1)) {
        int regionsIndex = lua_gettop(L);

        lua_rawgeti(L, regionsIndex, currentFrame);
        int regionIndex = lua_gettop(L);

        float values[4];

        for (int component = 1; component <= 4; ++component) {
            lua_rawgeti(L, regionIndex, component);

            values[component - 1] = (float) lua_tonumber(L, -1);

            lua_pop(L, 1);
        }

        lua_pop(L, 2); // region and regions

        sourceRect.x = values[0];
        sourceRect.y = values[1];
        sourceRect.w = values[2];
        sourceRect.h = values[3];

        srcRectPointer = &sourceRect;

        lua_rawgetfield(L, 1, "textureId");

        textureId = lua_tointeger(L, -1);
        lua_pop(L, 1);
    } else {
        lua_pop(L, 1);

        lua_rawgeti(L, 1, currentFrame);

        textureId = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    lua_rawgetfield(L, 1, "x");
    lua_rawgetfield(L, 1, "y");
    lua_rawgetfield(L, 1, "w");
    lua_rawgetfield(L, 1, "h");
    lua_rawgetfield(L, 1, "r");
    lua_rawgetfield(L, 1, "flipH");
    lua_rawgetfield(L, 1, "flipV");

    float x = lua_tonumber(L, -7);
    float y = lua_tonumber(L, -6);
    float w = lua_tonumber(L, -5);
    float h = lua_tonumber(L, -4);
    int r = lua_tonumber(L, -3);
    bool flipH = lua_toboolean(L, -2);
    bool flipV = lua_toboolean(L, -1);

    lua_pop(L, 7);

    SDL_Texture* texture = getTextureById(textureId);

    if (r == 0.0 && !flipH && !flipV) {
        if (currentTransform.isDefault()) {
            SDL_FRect drawingRect{x, y, w, h};
            SDL_RenderTexture(renderer, getTextureById(textureId), srcRectPointer, &drawingRect);
        } else {
            SDL_FPoint origin = transformPoint(x, y);

            SDL_FPoint right = {
                origin.x + w * currentTransform.xAxisX,
                origin.y + w * currentTransform.xAxisY
            };

            SDL_FPoint down = {
                origin.x + h * currentTransform.yAxisX,
                origin.y + h * currentTransform.yAxisY
            };

            SDL_RenderTextureAffine(
                renderer,
                texture,
                srcRectPointer,
                &origin,
                &right,
                &down
            );
        }
    } else {
        float pivotX = w / 2.0f;
        float pivotY = h / 2.0f;

        SDL_FPoint topLeft = transformSpritePoint(0, 0, x, y, pivotX, pivotY, r);
        SDL_FPoint topRight = transformSpritePoint(w, 0, x, y, pivotX, pivotY, r);
        SDL_FPoint bottomLeft = transformSpritePoint(0, h, x, y, pivotX, pivotY, r);
        SDL_FPoint bottomRight = transformSpritePoint(w, h, x, y, pivotX, pivotY, r);

        SDL_FPoint origin;
        SDL_FPoint right;
        SDL_FPoint down;

        if (flipH && flipV) {
            origin = bottomRight;
            right = bottomLeft;
            down = topRight;
        }
        else if (flipH) {
            origin = topRight;
            right = topLeft;
            down = bottomRight;
        }
        else if (flipV) {
            origin = bottomLeft;
            right = bottomRight;
            down = topLeft;
        }
        else {
            origin = topLeft;
            right = topRight;
            down = bottomLeft;
        }

        SDL_RenderTextureAffine(
            renderer,
            texture,
            srcRectPointer,
            &origin,
            &right,
            &down
        );
    }

    return 0;
}

int setPositionA(lua_State *L) {
    // self at index 1

    lua_pushinteger(L, lua_tonumber(L, 2));
    lua_setfield(L, 1, "x");
    lua_pushinteger(L, lua_tonumber(L, 3));
    lua_setfield(L, 1, "y");

    return 0;
}

int setSizeA(lua_State *L) {
    // self at index 1

    lua_pushinteger(L, lua_tonumber(L, 2));
    lua_setfield(L, 1, "w");
    lua_pushinteger(L, lua_tonumber(L, 3));
    lua_setfield(L, 1, "h");

    return 0;
}

int setRotationA(lua_State *L) {
    // self at index 1

    lua_pushinteger(L, lua_tointeger(L, 2));
    lua_setfield(L, 1, "r");

    return 0;
}

int flipHorizontalA(lua_State *L) {
    // self at index 1

    lua_pushboolean(L, lua_toboolean(L, 2));
    lua_setfield(L, 1, "flipH");

    return 0;
}

int flipVerticalA(lua_State *L) {
    // self at index 1

    lua_pushboolean(L, lua_toboolean(L, 2));
    lua_setfield(L, 1, "flipV");

    return 0;
}

int getTextureIdA(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "textureId");
    return 1;
}

int getPositionA(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "x");
    lua_rawgetfield(L, 1, "y");
    return 2;
}

int getSizeA(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "w");
    lua_rawgetfield(L, 1, "h");
    return 2;
}

int getRotationA(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "r");
    return 1;
}

int getFlipHorizontalA(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "flipH");
    return 1;
}

int getFlipVerticalA(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "flipV");
    return 1;
}

void pushASpriteClass(lua_State *L) {
    lua_newtable(L);

    lua_pushvalue(L, -1);

    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushvalue(L, -1);
    lua_pushcclosure(L, animatedSpriteNew, "animatedSpriteNew", 1);
    lua_setfield(L, -2, "new");

    lua_pushvalue(L, -1);
    lua_pushcclosure(L, fromSpritesheet, "fromSpritesheet", 1);
    lua_setfield(L, -2, "fromSpritesheet");

    lua_pushcfunction(L, drawA, "drawA");
    lua_setfield(L, -2, "draw");

    lua_pushcfunction(L, setPositionA, "setPositionA");
    lua_setfield(L, -2, "setPosition");

    lua_pushcfunction(L, setSizeA, "setSizeA");
    lua_setfield(L, -2, "setSize");

    lua_pushcfunction(L, setRotationA, "setRotationA");
    lua_setfield(L, -2, "setRotation");

    lua_pushcfunction(L, flipHorizontalA, "flipHorizontalA");
    lua_setfield(L, -2, "flipHorizontal");

    lua_pushcfunction(L, flipVerticalA, "flipVerticalA");
    lua_setfield(L, -2, "flipVertical");

    lua_pushcfunction(L, getTextureIdA, "getTextureId");
    lua_setfield(L, -2, "getTextureId");

    lua_pushcfunction(L, getPositionA, "getPositionA");
    lua_setfield(L, -2, "getPosition");

    lua_pushcfunction(L, getSizeA, "getSizeA");
    lua_setfield(L, -2, "getSize");

    lua_pushcfunction(L, getRotationA, "getRotationA");
    lua_setfield(L, -2, "getRotation");

    lua_pushcfunction(L, getFlipHorizontalA, "getFlipHorizontalA");
    lua_setfield(L, -2, "getFlipHorizontal");

    lua_pushcfunction(L, getFlipVerticalA, "getFlipVerticalA");
    lua_setfield(L, -2, "getFlipVertical");

    // class table remains on stack
}

void registerAnimatedSpriteObject(ResourceState* state) {
    if (renderer == nullptr) {
        renderer = state->getRenderer();
    }
    lua_State* L = state->getL();

    pushASpriteClass(L);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "AnimatedSprite");
}
