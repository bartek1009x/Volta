#include "graphics.hpp"

#include <cmath>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <algorithm>

#include <SDL3/SDL_rect.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_render.h>

#include "../dependencies/luau/VM/include/lualib.h"

using namespace std;

static SDL_Renderer *renderer = nullptr;
static ResourceState* resourceState = nullptr;
static unordered_map<int, SDL_Texture*> loadedTextures;
Uint32 textureIDCounter = 0;
unordered_map<int, TTF_Font*> loadedFonts;
Uint32 fontIDCounter = 0;
TransformationStack transformStack{10};
Transform currentTransform{
    .xAxisX = 1,
    .xAxisY = 0,

    .yAxisX = 0,
    .yAxisY = 1,

    .originX = 0,
    .originY = 0
};

SDL_Texture* getTextureById(int id) {
    return loadedTextures[id];
}

SDL_FPoint transformPoint(float x, float y) {
    return {
        currentTransform.originX +
            x * currentTransform.xAxisX +
            y * currentTransform.yAxisX,

        currentTransform.originY +
            x * currentTransform.xAxisY +
            y * currentTransform.yAxisY
    };
}

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
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float w = lua_tonumber(L, 3);
    float h = lua_tonumber(L, 4);

    SDL_FPoint topLeft = transformPoint(x, y);
    SDL_FPoint topRight = transformPoint(x + w, y);
    SDL_FPoint bottomRight = transformPoint(x + w, y + h);
    SDL_FPoint bottomLeft = transformPoint(x, y + h);

    if (lua_toboolean(L, 5)) {
        SDL_Color color;
        SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
        SDL_FColor fColor = {(float) color.r, (float) color.g, (float) color.b, (float) color.a};

        SDL_Vertex vertices[] = {
            {topLeft, fColor, {0, 0}},
            {topRight, fColor, {0, 0}},
            {bottomRight, fColor, {0, 0}},
            {bottomLeft, fColor, {0, 0}}
        };

        int indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
    } else {
        SDL_FPoint points[] = {topLeft, topRight, bottomRight, bottomLeft, topLeft};

        SDL_RenderLines(renderer, points, 5);
    }

    return 0;
}

int roundUpToMultipleOfEight(int v) {
    return (v + (8 - 1)) & -8;
}

void renderFillCircle(float centerX, float centerY, float radius) {
    constexpr int segments = 48;

    SDL_Color color;
    SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
    SDL_FColor fColor = {(float) color.r, (float) color.g, (float) color.b, (float) color.a};

    SDL_Vertex vertices[segments + 1];

    vertices[0] = {
        transformPoint(centerX, centerY),
        fColor,
        {0, 0}
    };

    for (int i = 0; i < segments; i++) {
        float angle = (float) i / (float) segments * 2.0f * SDL_PI_F;

        float localX = centerX + cosf(angle) * radius;
        float localY = centerY + sinf(angle) * radius;

        vertices[i + 1] = {
            transformPoint(localX, localY),
            fColor,
            {0, 0}
        };
    }

    int indices[segments * 3];

    for (int i = 0; i < segments; i++) {
        int next = (i + 1) % segments;

        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = next + 1;
    }

    SDL_RenderGeometry(renderer, nullptr, vertices, segments + 1, indices, segments * 3);
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
        points[drawCount + 0] = transformPoint(centerX + x, centerY - y);
        points[drawCount + 1] = transformPoint(centerX + x, centerY + y);
        points[drawCount + 2] = transformPoint(centerX - x, centerY - y);
        points[drawCount + 3] = transformPoint(centerX - x, centerY + y);
        points[drawCount + 4] = transformPoint(centerX + y, centerY - x);
        points[drawCount + 5] = transformPoint(centerX + y, centerY + x);
        points[drawCount + 6] = transformPoint(centerX - y, centerY - x);
        points[drawCount + 7] = transformPoint(centerX - y, centerY + x);

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

int loadImagePath(lua_State *L, const char* path) {
    std::filesystem::path finalPath = resourceState->getMainPath() / path;
    const std::string stringPath = finalPath.string(); // we need to convert it here, because if we don't, path.c_str() will return const wchar_t* on windows (we need const char*)
    const char* pathCStr = stringPath.c_str();
    SDL_Texture* texture = IMG_LoadTexture(renderer, pathCStr);
    if (texture == nullptr) {
        luaL_error(L, "Could not load texture: %s", pathCStr);
        return 0;
    }

    loadedTextures[textureIDCounter] = texture;

    return textureIDCounter++;
}

int setTextureScaleMode(lua_State *L) {
    int index = lua_tonumber(L, 1);
    std::string modeS = lua_tostring(L, 2);
    std::transform(modeS.begin(), modeS.end(), modeS.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    SDL_ScaleMode mode;

    if (modeS == "nearest") {
        mode = SDL_SCALEMODE_NEAREST;
    } else if (modeS == "linear") {
        mode = SDL_SCALEMODE_LINEAR;
    } else if (modeS == "pixelart") {
        mode = SDL_SCALEMODE_PIXELART;
    } else {
        luaL_error(L, "%s is not a valid scale mode.", modeS.c_str());
        return 0;
    }
    SDL_SetTextureScaleMode(loadedTextures[index], mode);

    return 0;
}

int loadImage(lua_State *L) {
    lua_pushnumber(L, loadImagePath(L, lua_tostring(L, 1)));

    return 1;
}

int unloadImage(lua_State *L) {
    int index = lua_tonumber(L, 1);
    SDL_DestroyTexture(loadedTextures[index]);
    loadedTextures.erase(index);

    return 0;
}

int drawImage(lua_State *L) {
    int textureId = lua_tonumber(L, 1);

    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float width = lua_tonumber(L, 4);
    float height = lua_tonumber(L, 5);

    if (currentTransform.isDefault()) {
        SDL_FRect drawingRect{x, y, width, height};
        SDL_RenderTexture(renderer, loadedTextures[lua_tonumber(L, 1)], nullptr, &drawingRect);
    } else {
        SDL_FPoint origin = transformPoint(x, y);

        SDL_FPoint right = {
            origin.x + width * currentTransform.xAxisX,
            origin.y + width * currentTransform.xAxisY
        };

        SDL_FPoint down = {
            origin.x + height * currentTransform.yAxisX,
            origin.y + height * currentTransform.yAxisY
        };

        SDL_RenderTextureAffine(
            renderer,
            loadedTextures[textureId],
            nullptr,
            &origin,
            &right,
            &down
        );
    }

    return 0;
}

int loadFont(lua_State *L) {
    const char* path = lua_tostring(L, 1);
    std::filesystem::path finalPath = resourceState->getMainPath() / path;
    const std::string stringPath = finalPath.string(); // we need to convert it here, because if we don't, path.c_str() will return const wchar_t* on windows (we need const char*)
    const char* pathCStr = stringPath.c_str();
    TTF_Font* font = TTF_OpenFont(pathCStr, lua_tonumber(L, 2));
    if (font == nullptr) {
        luaL_error(L, "Could not load font: %s, error: %s", pathCStr, SDL_GetError());
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

    float x = lua_tonumber(L, 3);
    float y = lua_tonumber(L, 4);

    if (currentTransform.isDefault()) {
        SDL_FRect drawingRect{x, y, width, height};
        SDL_RenderTexture(renderer, texture, nullptr, &drawingRect);
    } else {
        SDL_FPoint origin = transformPoint(x, y);

        SDL_FPoint right = {
            origin.x + width * currentTransform.xAxisX,
            origin.y + width * currentTransform.xAxisY
        };

        SDL_FPoint down = {
            origin.x + height * currentTransform.yAxisX,
            origin.y + height * currentTransform.yAxisY
        };

        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture, color.a);
        SDL_RenderTextureAffine(
            renderer,
            texture,
            nullptr,
            &origin,
            &right,
            &down
        );
    }

    return 0;
}

int pushCoord(lua_State *L) {
    if (!transformStack.push(currentTransform)) {
        luaL_error(L, "The transformation stack is full.");
    }

    return 0;
}

int translateCoord(lua_State *L) {
    float dx = lua_tonumber(L, 1);
    float dy = lua_tonumber(L, 2);

    currentTransform.originX +=
        dx * currentTransform.xAxisX +
        dy * currentTransform.yAxisX;

    currentTransform.originY +=
        dx * currentTransform.xAxisY +
        dy * currentTransform.yAxisY;

    return 0;
}

int scaleCoord(lua_State *L) {
    float sx = lua_tonumber(L, 1);
    float sy = lua_tonumber(L, 2);

    currentTransform.xAxisX *= sx;
    currentTransform.xAxisY *= sx;

    currentTransform.yAxisX *= sy;
    currentTransform.yAxisY *= sy;

    return 0;
}

int rotateCoord(lua_State *L) {
    float radians = lua_tonumber(L, 1);

    float c = cos(radians);
    float s = sin(radians);

    float oldXAxisX = currentTransform.xAxisX;
    float oldXAxisY = currentTransform.xAxisY;
    float oldYAxisX = currentTransform.yAxisX;
    float oldYAxisY = currentTransform.yAxisY;

    currentTransform.xAxisX =
        oldXAxisX * c + oldYAxisX * s;

    currentTransform.xAxisY =
        oldXAxisY * c + oldYAxisY * s;

    currentTransform.yAxisX =
        -oldXAxisX * s + oldYAxisX * c;

    currentTransform.yAxisY =
        -oldXAxisY * s + oldYAxisY * c;

    return 0;
}

int shearCoord(lua_State *L) {
    float shearX = lua_tonumber(L, 1);
    float shearY = lua_tonumber(L, 2);

    float oldXAxisX = currentTransform.xAxisX;
    float oldXAxisY = currentTransform.xAxisY;
    float oldYAxisX = currentTransform.yAxisX;
    float oldYAxisY = currentTransform.yAxisY;

    currentTransform.xAxisX =
        oldXAxisX + shearY * oldYAxisX;

    currentTransform.xAxisY =
        oldXAxisY + shearY * oldYAxisY;

    currentTransform.yAxisX =
        oldYAxisX + shearX * oldXAxisX;

    currentTransform.yAxisY =
        oldYAxisY + shearX * oldXAxisY;

    return 0;
}

int popCoord(lua_State *L) {
    if (transformStack.isEmpty()) {
        luaL_error(L, "The transformation stack is empty, cannot pop.");
        return 0;
    }

    currentTransform = *transformStack.peek();
    transformStack.pop();
    return 0;
}

static const luaL_Reg graphics_lib[] = {
    {"setCursorVisibility", setCursorVisibility},
    {"setVsync", setVsync},
    {"setDrawColor", setDrawColor},
    {"clear", clear},
    {"drawRect", drawRect},
    {"drawCircle", drawCircle},
    {"loadImage", loadImage},
    {"setTextureScaleMode", setTextureScaleMode},
    {"unloadImage", unloadImage},
    {"drawImage", drawImage},
    {"loadFont", loadFont},
    {"unloadFont", unloadFont},
    {"drawText", drawText},
    {"pushCoord", pushCoord},
    {"translateCoord", translateCoord},
    {"scaleCoord", scaleCoord},
    {"rotateCoord", rotateCoord},
    {"shearCoord", shearCoord},
    {"popCoord", popCoord},
    {nullptr, nullptr},
};

void registerGraphicsFunctions(ResourceState* state) {
    TTF_Init();
    resourceState = state;
    if (renderer == nullptr) {
        renderer = state->getRenderer();
    }
    lua_State* L = state->getL();

    luaL_register(L, "graphics", graphics_lib);
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