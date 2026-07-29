#include "graphics.hpp"

#include <iostream>
#include <filesystem>
#include <unordered_map>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_mouse.h>

#include "../dependencies/luau/VM/include/lualib.h"

using namespace std;

SDL_Renderer *renderer = nullptr;
static ResourceState* resourceState = nullptr;
unordered_map<int, SDL_Texture*> loadedTextures;
Uint32 textureIDCounter = 0;
SDL_FRect rect;

int setCursorVisibility(lua_State *L) {
    if (lua_toboolean(L, 1)) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
    return 0;
}

int setVsync(lua_State *L) {
    SDL_SetRenderVSync(renderer, lua_toboolean(L, 1) ? 1 : SDL_RENDERER_VSYNC_DISABLED);
    return 0;
}

int setDrawColor(lua_State *L) {
    SDL_SetRenderDrawColor(renderer, lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    return 0;
}

int clear(lua_State *L) {
    SDL_RenderClear(renderer);
    return 0;
}

int drawRect(lua_State *L) {
    rect.x = lua_tonumber(L, 1);
    rect.y = lua_tonumber(L, 2);
    rect.w = lua_tonumber(L, 3);
    rect.h = lua_tonumber(L, 4);

    if (lua_toboolean(L, 5)) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderRect(renderer, &rect);
    }

    return 0;
}

int roundUpToMultipleOfEight(int v)
{
    return (v + (8 - 1)) & -8;
}

int renderFillCircle(SDL_Renderer * renderer, int x, int y, int radius) {
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderLine(renderer, x - offsety, y + offsetx, x + offsety, y + offsetx);
        status += SDL_RenderLine(renderer, x - offsetx, y + offsety, x + offsetx, y + offsety);
        status += SDL_RenderLine(renderer, x - offsetx, y - offsety, x + offsetx, y - offsety);
        status += SDL_RenderLine(renderer, x - offsety, y - offsetx, x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx +=1;
        } else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        } else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

int drawCircle(lua_State *L) {
    float centerX = lua_tonumber(L, 1);
    float centerY = lua_tonumber(L, 2);
    int radius = lua_tonumber(L, 3);
    bool filled = lua_toboolean(L, 4);

    if (filled) {
        renderFillCircle(renderer, centerX, centerY, radius);
    } else {
        const int arrSize = roundUpToMultipleOfEight(radius * 8 * 35 / 49);
        SDL_FPoint points[arrSize];
        int drawCount = 0;

        const int32_t diameter = (radius * 2);

        int32_t x = (radius - 1);
        int32_t y = 0;
        int32_t tx = 1;
        int32_t ty = 1;
        int32_t error = (tx - diameter);

        while( x >= y ) {
            // Each of the following renders an octant of the circle
            points[drawCount+0] = {centerX + x, centerY - y};
            points[drawCount+1] = {centerX + x, centerY + y};
            points[drawCount+2] = {centerX - x, centerY - y};
            points[drawCount+3] = {centerX - x, centerY + y};
            points[drawCount+4] = {centerX + y, centerY - x};
            points[drawCount+5] = {centerX + y, centerY + x};
            points[drawCount+6] = {centerX - y, centerY - x};
            points[drawCount+7] = {centerX - y, centerY + x};

            drawCount += 8;

            if (error <= 0) {
                ++y;
                error += ty;
                ty += 2;
            }

            if (error > 0) {
                --x;
                tx += 2;
                error += (tx - diameter);
            }
        }

        SDL_RenderPoints(renderer, points, drawCount);
    }

    return 0;
}

int loadImage(lua_State *L) {
    const char* path = lua_tostring(L, 1);
    std::filesystem::path finalPath = resourceState->getMainPath() / path;
    SDL_Texture* texture = IMG_LoadTexture(renderer, finalPath.c_str());
    if (texture == nullptr) {
        luaL_error(L, "Could not load texture: %s", finalPath.c_str());
        return 0;
    }

    loadedTextures[textureIDCounter] = texture;

    lua_pushnumber(L, textureIDCounter);

    textureIDCounter++;

    return 1;
}

int unloadImage(lua_State *L) {
    int index = lua_tonumber(L, 1);
    SDL_DestroyTexture(loadedTextures[index]);
    loadedTextures.erase(index);

    return 0;
}

int drawImage(lua_State *L) {
    rect.x = lua_tonumber(L, 2);
    rect.y = lua_tonumber(L, 3);
    rect.w = lua_tonumber(L, 4);
    rect.h = lua_tonumber(L, 5);
    SDL_RenderTexture(renderer, loadedTextures[lua_tonumber(L, 1)], nullptr, &rect);
    return 0;
}

void registerGraphicsFunctions(ResourceState* state) {
    resourceState = state;
    if (renderer == nullptr) {
        renderer = state->getRenderer();
    }
    lua_State* L = state->getL();

    lua_createtable(L, 1, 0);

    lua_pushcfunction(L, setCursorVisibility, "setCursorVisibility");
    lua_setfield(L, -2, "setCursorVisibility");
    lua_pushcfunction(L, setVsync, "setVsync");
    lua_setfield(L, -2, "setVsync");
    lua_pushcfunction(L, setDrawColor, "setDrawColor");
    lua_setfield(L, -2, "setDrawColor");
    lua_pushcfunction(L, clear, "clear");
    lua_setfield(L, -2, "clear");
    lua_pushcfunction(L, drawRect, "drawRect");
    lua_setfield(L, -2, "drawRect");
    lua_pushcfunction(L, drawCircle, "drawCircle");
    lua_setfield(L, -2, "drawCircle");
    lua_pushcfunction(L, loadImage, "loadImage");
    lua_setfield(L, -2, "loadImage");
    lua_pushcfunction(L, unloadImage, "unloadImage");
    lua_setfield(L, -2, "unloadImage");
    lua_pushcfunction(L, drawImage, "drawImage");
    lua_setfield(L, -2, "drawImage");

    lua_setglobal(L, "graphics");
}