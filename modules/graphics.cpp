#include "graphics.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <iostream>
#include <filesystem>
#include <unordered_map>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../dependencies/luau/VM/include/lualib.h"

using namespace std;

SDL_Renderer *renderer = nullptr;
static ResourceState* resourceState = nullptr;
unordered_map<int, SDL_Texture*> loadedTextures;
Uint32 textureIDCounter = 0;
unordered_map<int, TTF_Font*> loadedFonts;
Uint32 fontIDCounter = 0;
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
    if (lua_isnumber(L, 1)) {
        SDL_SetRenderDrawColor(renderer, lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    } else {
        luaL_checktype(L, 1, LUA_TTABLE);

        lua_rawgetfield(L, 1, "r");
        lua_rawgetfield(L, 1, "g");
        lua_rawgetfield(L, 1, "b");
        lua_rawgetfield(L, 1, "a");

        Uint8 r = lua_tointeger(L, -4);
        Uint8 g = lua_tointeger(L, -3);
        Uint8 b = lua_tointeger(L, -2);
        Uint8 a = lua_tointeger(L, -1);

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }
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

int roundUpToMultipleOfEight(int v) {
    return (v + (8 - 1)) & -8;
}

void renderFillCircle(float x, float y, int radius) {
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
}

void renderCircle(float centerX, float centerY, int radius) {
    const int arrSize = roundUpToMultipleOfEight(radius * 8 * 35 / 49);
    SDL_FPoint points[arrSize];
    int drawCount = 0;

    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
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

int drawCircle(lua_State *L) {
    float centerX = lua_tonumber(L, 1);
    float centerY = lua_tonumber(L, 2);
    int radius = lua_tonumber(L, 3);
    bool filled = lua_toboolean(L, 4);

    if (filled) {
        renderFillCircle(centerX, centerY, radius);
    } else {
        renderCircle(centerX, centerY, radius);
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

int loadFont(lua_State *L) {
    const char* path = lua_tostring(L, 1);
    std::filesystem::path finalPath = resourceState->getMainPath() / path;
    TTF_Font* font = TTF_OpenFont(finalPath.c_str(), lua_tonumber(L, 2));
    if (font == nullptr) {
        luaL_error(L, "Could not load font: %s, error: %s", finalPath.c_str(), SDL_GetError());
        return 0;
    }

    loadedFonts[fontIDCounter] = font;

    lua_pushnumber(L, fontIDCounter);

    fontIDCounter++;

    return 1;
}

int unloadFont(lua_State *L) {
    int index = lua_tonumber(L, 1);
    TTF_CloseFont(loadedFonts[index]);
    loadedFonts.erase(index);

    return 0;
}

struct textInfo {
    Uint32 fontId;
    SDL_Texture* texture;
    float width;
    float height;
    int lastFrameUsed;
} typedef textInfo;

unordered_map<std::string, textInfo> textCache;

extern Uint32 CURRENT_FRAME;

static const SDL_Color white = {255, 255, 255, 255};
int drawText(lua_State *L) {
    SDL_Color color;
    SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
    SDL_Texture *texture;
    float width, height;

    const char* text = lua_tostring(L, 2);
    Uint32 fontId = lua_tonumber(L, 1);
    bool dontCache = lua_toboolean(L, 5);
    printf("%b", dontCache);

    auto it = textCache.find(text);
    if (!dontCache && it != textCache.end()) {
        if (it->second.fontId == fontId) {
            texture = it->second.texture;
            width = it->second.width;
            height = it->second.height;
            it->second.lastFrameUsed = CURRENT_FRAME;
        } else {
            SDL_DestroyTexture(it->second.texture);

            SDL_Surface *surface = TTF_RenderText_Blended(loadedFonts[fontId], text, 0, white);
            SDL_Texture *newTexture = SDL_CreateTextureFromSurface(renderer, surface);
            textInfo info = {.fontId = fontId, .texture = newTexture, .width = surface->w, .height = surface->h, .lastFrameUsed = CURRENT_FRAME};
            textCache[text] = info;

            width = surface->w;
            height = surface->h;

            SDL_DestroySurface(surface);

            texture = newTexture;
        }
    } else {
        SDL_Surface *surface = TTF_RenderText_Blended(loadedFonts[fontId], text, 0, white);
        SDL_Texture *newTexture = SDL_CreateTextureFromSurface(renderer, surface);

        width = surface->w;
        height = surface->h;

        SDL_DestroySurface(surface);

        texture = newTexture;

        if (!dontCache) {
            textInfo info = {.fontId = fontId, .texture = newTexture, .width = width, .height = height, .lastFrameUsed = CURRENT_FRAME};
            textCache[text] = info;
        }
    }

    rect.x = lua_tonumber(L, 3);
    rect.y = lua_tonumber(L, 4);
    rect.w = width;
    rect.h = height;

    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);
    SDL_RenderTexture(renderer, texture, nullptr, &rect);

    return 0;
}

void registerGraphicsFunctions(ResourceState* state) {
    TTF_Init();
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
    lua_pushcfunction(L, loadFont, "loadFont");
    lua_setfield(L, -2, "loadFont");
    lua_pushcfunction(L, unloadFont, "unloadFont");
    lua_setfield(L, -2, "unloadFont");
    lua_pushcfunction(L, drawText, "drawText");
    lua_setfield(L, -2, "drawText");

    lua_setreadonly(L, -1, 1);

    lua_setfield(L, -2, "graphics");
}

void updateFontTextCache() {
    for (auto it = textCache.begin(); it != textCache.end();) {
        if (CURRENT_FRAME - it->second.lastFrameUsed >= 20000) {
            SDL_DestroyTexture(it->second.texture);
            it = textCache.erase(it);
        } else {
            ++it;
        }
    }
}