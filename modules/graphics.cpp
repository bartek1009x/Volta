#include "graphics.hpp"

#include <cmath>
#include <cstring>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <limits>
#include <cstdint>
#include <cctype>
#include <memory>
#include <unordered_set>

#include <SDL3/SDL_rect.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_gpu.h>

#include "../dependencies/luau/VM/include/lualib.h"
#include "../dependencies/SDL_shadercross/include/SDL3_shadercross/SDL_shadercross.h"

struct ShaderUniformReflection;

typedef struct {
    SDL_GPUShader* shader;
    SDL_GPURenderState* renderState;
    std::shared_ptr<ShaderUniformReflection> uniformReflection;
    std::unordered_map<Uint32, std::vector<Uint8>> uniformBuffers;
} shaderData;

using namespace std;

static SDL_Renderer *renderer;
static ResourceState* resourceState = nullptr;
static unordered_map<int, SDL_Texture*> loadedTextures;
Uint32 textureIDCounter = 0;
unordered_map<int, TTF_Font*> loadedFonts;
Uint32 fontIDCounter = 0;
unordered_map<int, shaderData> createdShaders;
Uint32 shaderIDCounter = 0;
static int currentGraphicsPipelineId = -1;

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

int isCursorVisible(lua_State *L) {
    lua_pushboolean(L, SDL_CursorVisible());
    return 1;
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

int isVsyncEnabled(lua_State *L) {
    int vsync = 0;
    SDL_GetRenderVSync(renderer, &vsync);
    lua_pushboolean(L, vsync == 1 || vsync == -1);
    return 1;
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
        SDL_FColor fColor = {(float) color.r / 255, (float) color.g / 255, (float) color.b / 255, (float) color.a / 255};

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
    SDL_FColor fColor = {(float) color.r / 255, (float) color.g / 255, (float) color.b / 255, (float) color.a / 255};

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
    lua_pushinteger(L, loadImagePath(L, lua_tostring(L, 1)));

    return 1;
}

int unloadImage(lua_State *L) {
    int index = lua_tonumber(L, 1);
    SDL_DestroyTexture(loadedTextures[index]);
    loadedTextures.erase(index);

    return 0;
}

int drawImage(lua_State *L) {
    int textureId = lua_tointeger(L, 1);

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

int drawImageRegion(lua_State *L) {
    int textureId = lua_tointeger(L, 1);

    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);
    float width = lua_tonumber(L, 4);
    float height = lua_tonumber(L, 5);

    float regionX = lua_tonumber(L, 6);
    float regionY = lua_tonumber(L, 7);
    float regionW = lua_tonumber(L, 8);
    float regionH = lua_tonumber(L, 9);

    SDL_FRect srcRect{regionX, regionY, regionW, regionH};

    if (currentTransform.isDefault()) {
        SDL_FRect drawingRect{x, y, width, height};
        SDL_RenderTexture(renderer, loadedTextures[lua_tonumber(L, 1)], &srcRect, &drawingRect);
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
            &srcRect,
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

    lua_pushinteger(L, fontIDCounter);

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
    Uint32 lastFrameUsed;
    Uint32 lifetime;
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
    Uint32 fontId = lua_tointeger(L, 1);
    Uint32 cacheLifetime = 10000;
    if (lua_type(L, 5) == LUA_TINTEGER) {
        cacheLifetime = lua_tointeger(L, 5);
    }

    auto it = textCache.find(text);
    if (it != textCache.end()) {
        if (it->second.fontId == fontId) {
            texture = it->second.texture;
            width = it->second.width;
            height = it->second.height;
            it->second.lastFrameUsed = CURRENT_FRAME;
        } else {
            SDL_DestroyTexture(it->second.texture);

            SDL_Surface *surface = TTF_RenderText_Blended(loadedFonts[fontId], text, 0, white);
            SDL_Texture *newTexture = SDL_CreateTextureFromSurface(renderer, surface);
            textInfo info = {.fontId = fontId, .texture = newTexture, .width = surface->w, .height = surface->h, .lastFrameUsed = CURRENT_FRAME, .lifetime = cacheLifetime};
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

        textInfo info = {.fontId = fontId, .texture = newTexture, .width = width, .height = height, .lastFrameUsed = CURRENT_FRAME, .lifetime = cacheLifetime};
        textCache[text] = info;
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
    transformStack.push(currentTransform);
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

int setScissor(lua_State *L) {
    if (lua_isnoneornil(L, 1)) {
        SDL_SetRenderClipRect(renderer, nullptr);
    } else {
        SDL_Rect rect{lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4)};
        SDL_SetRenderClipRect(renderer, &rect);
    }
    return 0;
}

int getScissor(lua_State *L) {
    if (!SDL_RenderClipEnabled(renderer)) {
        return 0;
    }

    SDL_Rect rect;
    SDL_GetRenderClipRect(renderer, &rect);

    lua_pushinteger(L, rect.x);
    lua_pushinteger(L, rect.y);
    lua_pushinteger(L, rect.w);
    lua_pushinteger(L, rect.h);
    return 4;
}

static std::shared_ptr<ShaderUniformReflection> reflectShaderUniforms(const Uint8* bytecode, size_t bytecodeSize, std::string& error);

int createShader(lua_State *L) {
    SDL_GPUDevice* device = SDL_GetGPURendererDevice(renderer);

    const char* hlslCode = lua_tostring(L, 1);
    const char* hlslMain = luaL_optstring(L, 2, "main");

    SDL_ShaderCross_HLSL_Info hlslInfo = {
        .source = hlslCode,
        .entrypoint = hlslMain,
        .include_dir = nullptr,
        .defines = nullptr,
        .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
        .props = 0
    };

    size_t spirvSize = 0;
    void* spirvBytecode = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlslInfo, &spirvSize);

    if (!spirvBytecode) {
        luaL_error(L, "Could not compile shader: %s", SDL_GetError());
        return 0;
    }

    SDL_ShaderCross_SPIRV_Info spirvInfo = {
        .bytecode = (Uint8*) spirvBytecode,
        .bytecode_size = spirvSize,
        .entrypoint = hlslMain,
        .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
        .props = 0
    };

    const SDL_ShaderCross_GraphicsShaderMetadata* metadata = SDL_ShaderCross_ReflectGraphicsSPIRV((Uint8*) spirvBytecode, spirvSize, 0);
    if (!metadata) {
        SDL_free(spirvBytecode);
        luaL_error(L, "Could not reflect shader: %s", SDL_GetError());
        return 0;
    }

    std::string uniformReflectionError;
    std::shared_ptr<ShaderUniformReflection> uniformReflection = reflectShaderUniforms((const Uint8*) spirvBytecode, spirvSize, uniformReflectionError);
    if (!uniformReflection) {
        SDL_free((void*) metadata);
        SDL_free(spirvBytecode);
        luaL_error(L, "Could not reflect shader uniforms: %s", uniformReflectionError.c_str());
        return 0;
    }

    SDL_GPUShader* shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(device, &spirvInfo, &metadata->resource_info, 0);

    SDL_free(spirvBytecode);
    SDL_free((void*) metadata);

    if (!shader) {
        luaL_error(L, "Could not create GPU shader: %s", SDL_GetError());
        return 0;
    }

    SDL_GPURenderStateCreateInfo info = {
        .fragment_shader = shader,

        .num_sampler_bindings = 0,
        .sampler_bindings = nullptr,

        .num_storage_textures = 0,
        .storage_textures = nullptr,

        .num_storage_buffers = 0,
        .storage_buffers = nullptr,

        .props = 0
    };

    SDL_GPURenderState* state = SDL_CreateGPURenderState(renderer, &info);

    shaderData data = {
        .shader = shader,
        .renderState = state,
        .uniformReflection = uniformReflection,
        .uniformBuffers = {}
    };
    createdShaders[shaderIDCounter] = data;
    lua_pushinteger(L, shaderIDCounter);

    shaderIDCounter++;

    return 1;
}

int setShader(lua_State *L) {
    if (lua_isnoneornil(L, 1)) {
        if (!SDL_SetGPURenderState(renderer, nullptr)) {
            luaL_error(L, "Could not reset GPU render state: %s", SDL_GetError());
            return 0;
        }

        return 0;
    }

    int shaderId = lua_tointeger(L, 1);
    shaderData data = createdShaders[shaderId];

    if (!SDL_SetGPURenderState(renderer, data.renderState)) {
        luaL_error(L, "Could not set GPU render state: %s", SDL_GetError());
        return 0;
    }

    return 0;
}

int destroyShader(lua_State *L) {
    int shaderId = lua_tointeger(L, 1);

    SDL_GPUDevice* device = SDL_GetGPURendererDevice(renderer);
    shaderData data = createdShaders[shaderId];

    SDL_DestroyGPURenderState(data.renderState);
    SDL_ReleaseGPUShader(device, data.shader);

    createdShaders.erase(shaderId);

    return 0;
}

enum class SpirvUniformTypeKind {
    Unknown,
    Bool,
    Int,
    Float,
    Vector,
    Matrix,
    Array,
    Struct,
    Pointer,
};

struct SpirvUniformType {
    SpirvUniformTypeKind kind = SpirvUniformTypeKind::Unknown;
    Uint32 width = 0;
    bool isSigned = false;
    Uint32 elementType = 0;
    Uint32 count = 0;
    Uint32 lengthId = 0;
    Uint32 storageClass = 0;
    std::vector<Uint32> memberTypes;
};

struct SpirvMemberDecoration {
    bool hasOffset = false;
    Uint32 offset = 0;
    bool rowMajor = false;
    bool colMajor = false;
    bool hasMatrixStride = false;
    Uint32 matrixStride = 0;
};

struct ShaderUniformDesc {
    Uint32 slot = 0;
    Uint32 offset = 0;
    Uint32 typeId = 0;
    Uint32 ownerStructType = 0;
    Uint32 ownerMemberIndex = 0;
};

struct ShaderUniformReflection {
    std::unordered_map<Uint32, SpirvUniformType> types;
    std::unordered_map<Uint32, Uint64> constants;
    std::unordered_map<Uint32, Uint32> arrayStrides;
    std::unordered_map<Uint64, SpirvMemberDecoration> memberDecorations;
    std::unordered_map<Uint64, std::string> memberNames;
    std::unordered_map<Uint32, std::string> names;
    std::unordered_map<std::string, ShaderUniformDesc> uniforms;
    std::unordered_set<std::string> ambiguousUniformNames;
    std::unordered_map<Uint32, Uint32> bufferSizes;
};

struct SpirvVariableInfo {
    Uint32 resultType = 0;
    Uint32 storageClass = 0;
};

static Uint64 spirvMemberKey(Uint32 structType, Uint32 memberIndex) {
    return ((Uint64) structType << 32) | memberIndex;
}

static std::string readSpirvString(const Uint32* instruction, Uint32 wordCount, Uint32 firstStringWord) {
    if (firstStringWord >= wordCount) {
        return {};
    }

    const char* chars = reinterpret_cast<const char*>(instruction + firstStringWord);
    const size_t maxBytes = (size_t) (wordCount - firstStringWord) * sizeof(Uint32);
    size_t length = 0;
    while (length < maxBytes && chars[length] != '\0') {
        ++length;
    }
    return std::string(chars, length);
}

static size_t uniformScalarSize(const SpirvUniformType& type) {
    if (type.kind == SpirvUniformTypeKind::Bool) {
        return 4;
    }
    return type.width / 8;
}

static size_t reflectedTypeSize(const ShaderUniformReflection& reflection, Uint32 typeId, const SpirvMemberDecoration* containingMember = nullptr);

static size_t reflectedStructSize(const ShaderUniformReflection& reflection, Uint32 structTypeId) {
    auto typeIt = reflection.types.find(structTypeId);
    if (typeIt == reflection.types.end() || typeIt->second.kind != SpirvUniformTypeKind::Struct) {
        return 0;
    }

    const SpirvUniformType& type = typeIt->second;
    size_t size = 0;
    for (Uint32 i = 0; i < type.memberTypes.size(); ++i) {
        auto decorationIt = reflection.memberDecorations.find(spirvMemberKey(structTypeId, i));
        if (decorationIt == reflection.memberDecorations.end() || !decorationIt->second.hasOffset) {
            continue;
        }
        const SpirvMemberDecoration& decoration = decorationIt->second;
        size = std::max(
            size,
            (size_t) decoration.offset + reflectedTypeSize(reflection, type.memberTypes[i], &decoration)
        );
    }

    return (size + 15u) & ~((size_t) 15u);
}

static size_t reflectedTypeSize(const ShaderUniformReflection& reflection, Uint32 typeId, const SpirvMemberDecoration* containingMember) {
    auto typeIt = reflection.types.find(typeId);
    if (typeIt == reflection.types.end()) {
        return 0;
    }

    const SpirvUniformType& type = typeIt->second;
    switch (type.kind) {
        case SpirvUniformTypeKind::Bool:
        case SpirvUniformTypeKind::Int:
        case SpirvUniformTypeKind::Float:
            return uniformScalarSize(type);

        case SpirvUniformTypeKind::Vector:
            return reflectedTypeSize(reflection, type.elementType) * type.count;

        case SpirvUniformTypeKind::Matrix: {
            auto columnIt = reflection.types.find(type.elementType);
            if (columnIt == reflection.types.end() || columnIt->second.kind != SpirvUniformTypeKind::Vector) {
                return 0;
            }
            const Uint32 rows = columnIt->second.count;
            const Uint32 columns = type.count;
            const Uint32 vectorCount = (containingMember && containingMember->rowMajor) ? rows : columns;
            const size_t packedVectorSize = reflectedTypeSize(reflection, type.elementType);
            const size_t stride = (containingMember && containingMember->hasMatrixStride)
                ? containingMember->matrixStride
                : ((packedVectorSize + 15u) & ~((size_t) 15u));
            return stride * vectorCount;
        }

        case SpirvUniformTypeKind::Array: {
            auto countIt = reflection.constants.find(type.lengthId);
            if (countIt == reflection.constants.end()) {
                return 0;
            }
            auto strideIt = reflection.arrayStrides.find(typeId);
            if (strideIt == reflection.arrayStrides.end()) {
                return 0;
            }
            return (size_t) strideIt->second * (size_t) countIt->second;
        }

        case SpirvUniformTypeKind::Struct:
            return reflectedStructSize(reflection, typeId);

        default:
            return 0;
    }
}

static std::shared_ptr<ShaderUniformReflection> reflectShaderUniforms(const Uint8* bytecode, size_t bytecodeSize, std::string& error) {
    if (!bytecode || bytecodeSize < 5 * sizeof(Uint32) || (bytecodeSize % sizeof(Uint32)) != 0) {
        error = "Invalid SPIR-V bytecode.";
        return nullptr;
    }

    const Uint32* words = reinterpret_cast<const Uint32*>(bytecode);
    const size_t totalWords = bytecodeSize / sizeof(Uint32);
    if (words[0] != 0x07230203u) {
        error = "Compiled shader is not valid SPIR-V.";
        return nullptr;
    }

    auto reflection = std::make_shared<ShaderUniformReflection>();
    std::unordered_map<Uint32, SpirvVariableInfo> variables;
    std::unordered_map<Uint32, Uint32> bindings;
    std::unordered_set<Uint32> blockStructs;

    size_t cursor = 5;
    while (cursor < totalWords) {
        const Uint32 instructionWord = words[cursor];
        const Uint16 opcode = (Uint16) (instructionWord & 0xFFFFu);
        const Uint16 wordCount = (Uint16) (instructionWord >> 16);
        if (wordCount == 0 || cursor + wordCount > totalWords) {
            error = "Malformed SPIR-V instruction stream.";
            return nullptr;
        }

        const Uint32* inst = words + cursor;
        switch (opcode) {
            case 5: { // OpName
                if (wordCount >= 3) {
                    reflection->names[inst[1]] = readSpirvString(inst, wordCount, 2);
                }
                break;
            }
            case 6: { // OpMemberName
                if (wordCount >= 4) {
                    reflection->memberNames[spirvMemberKey(inst[1], inst[2])] = readSpirvString(inst, wordCount, 3);
                }
                break;
            }
            case 20: { // OpTypeBool
                if (wordCount >= 2) {
                    SpirvUniformType type;
                    type.kind = SpirvUniformTypeKind::Bool;
                    type.width = 32;
                    reflection->types[inst[1]] = type;
                }
                break;
            }
            case 21: { // OpTypeInt
                if (wordCount >= 4) {
                    SpirvUniformType type;
                    type.kind = SpirvUniformTypeKind::Int;
                    type.width = inst[2];
                    type.isSigned = inst[3] != 0;
                    reflection->types[inst[1]] = type;
                }
                break;
            }
            case 22: { // OpTypeFloat
                if (wordCount >= 3) {
                    SpirvUniformType type;
                    type.kind = SpirvUniformTypeKind::Float;
                    type.width = inst[2];
                    reflection->types[inst[1]] = type;
                }
                break;
            }
            case 23: { // OpTypeVector
                if (wordCount >= 4) {
                    SpirvUniformType type;
                    type.kind = SpirvUniformTypeKind::Vector;
                    type.elementType = inst[2];
                    type.count = inst[3];
                    reflection->types[inst[1]] = type;
                }
                break;
            }
            case 24: { // OpTypeMatrix
                if (wordCount >= 4) {
                    SpirvUniformType type;
                    type.kind = SpirvUniformTypeKind::Matrix;
                    type.elementType = inst[2];
                    type.count = inst[3];
                    reflection->types[inst[1]] = type;
                }
                break;
            }
            case 28: { // OpTypeArray
                if (wordCount >= 4) {
                    SpirvUniformType type;
                    type.kind = SpirvUniformTypeKind::Array;
                    type.elementType = inst[2];
                    type.lengthId = inst[3];
                    reflection->types[inst[1]] = type;
                }
                break;
            }
            case 30: { // OpTypeStruct
                if (wordCount >= 2) {
                    SpirvUniformType type;
                    type.kind = SpirvUniformTypeKind::Struct;
                    for (Uint32 i = 2; i < wordCount; ++i) {
                        type.memberTypes.push_back(inst[i]);
                    }
                    reflection->types[inst[1]] = std::move(type);
                }
                break;
            }
            case 32: { // OpTypePointer
                if (wordCount >= 4) {
                    SpirvUniformType type;
                    type.kind = SpirvUniformTypeKind::Pointer;
                    type.storageClass = inst[2];
                    type.elementType = inst[3];
                    reflection->types[inst[1]] = type;
                }
                break;
            }
            case 43: { // OpConstant
                if (wordCount >= 4) {
                    Uint64 value = inst[3];
                    auto resultTypeIt = reflection->types.find(inst[1]);
                    if (resultTypeIt != reflection->types.end() && resultTypeIt->second.width == 64 && wordCount >= 5) {
                        value |= ((Uint64) inst[4] << 32);
                    }
                    reflection->constants[inst[2]] = value;
                }
                break;
            }
            case 59: { // OpVariable
                if (wordCount >= 4) {
                    variables[inst[2]] = SpirvVariableInfo{inst[1], inst[3]};
                }
                break;
            }
            case 71: { // OpDecorate
                if (wordCount >= 3) {
                    const Uint32 target = inst[1];
                    const Uint32 decoration = inst[2];
                    if (decoration == 2) { // Block
                        blockStructs.insert(target);
                    } else if (decoration == 6 && wordCount >= 4) { // ArrayStride
                        reflection->arrayStrides[target] = inst[3];
                    } else if (decoration == 33 && wordCount >= 4) { // Binding
                        bindings[target] = inst[3];
                    }
                }
                break;
            }
            case 72: { // OpMemberDecorate
                if (wordCount >= 4) {
                    SpirvMemberDecoration& decoration = reflection->memberDecorations[spirvMemberKey(inst[1], inst[2])];
                    switch (inst[3]) {
                        case 4: // RowMajor
                            decoration.rowMajor = true;
                            break;
                        case 5: // ColMajor
                            decoration.colMajor = true;
                            break;
                        case 7: // MatrixStride
                            if (wordCount >= 5) {
                                decoration.hasMatrixStride = true;
                                decoration.matrixStride = inst[4];
                            }
                            break;
                        case 35: // Offset
                            if (wordCount >= 5) {
                                decoration.hasOffset = true;
                                decoration.offset = inst[4];
                            }
                            break;
                        default:
                            break;
                    }
                }
                break;
            }
            default:
                break;
        }

        cursor += wordCount;
    }

    // SPIR-V storage class Uniform == 2. A uniform-buffer variable is a pointer
    // to a struct decorated Block. The Binding decoration is the SDL uniform slot.
    for (const auto& [variableId, variable] : variables) {
        if (variable.storageClass != 2) {
            continue;
        }

        auto pointerIt = reflection->types.find(variable.resultType);
        if (pointerIt == reflection->types.end() || pointerIt->second.kind != SpirvUniformTypeKind::Pointer) {
            continue;
        }

        const Uint32 structTypeId = pointerIt->second.elementType;
        auto structIt = reflection->types.find(structTypeId);
        if (structIt == reflection->types.end() ||
            structIt->second.kind != SpirvUniformTypeKind::Struct ||
            blockStructs.find(structTypeId) == blockStructs.end()) {
            continue;
        }

        auto bindingIt = bindings.find(variableId);
        if (bindingIt == bindings.end()) {
            continue;
        }
        const Uint32 slot = bindingIt->second;
        const Uint32 bufferSize = (Uint32) reflectedStructSize(*reflection, structTypeId);
        reflection->bufferSizes[slot] = std::max(reflection->bufferSizes[slot], bufferSize);

        std::string blockName;
        auto variableNameIt = reflection->names.find(variableId);
        if (variableNameIt != reflection->names.end()) {
            blockName = variableNameIt->second;
        }
        if (blockName.empty()) {
            auto structNameIt = reflection->names.find(structTypeId);
            if (structNameIt != reflection->names.end()) {
                blockName = structNameIt->second;
            }
        }

        for (Uint32 memberIndex = 0; memberIndex < structIt->second.memberTypes.size(); ++memberIndex) {
            const Uint64 key = spirvMemberKey(structTypeId, memberIndex);
            auto memberNameIt = reflection->memberNames.find(key);
            auto decorationIt = reflection->memberDecorations.find(key);
            if (memberNameIt == reflection->memberNames.end() || memberNameIt->second.empty() ||
                decorationIt == reflection->memberDecorations.end() || !decorationIt->second.hasOffset) {
                continue;
            }

            ShaderUniformDesc desc;
            desc.slot = slot;
            desc.offset = decorationIt->second.offset;
            desc.typeId = structIt->second.memberTypes[memberIndex];
            desc.ownerStructType = structTypeId;
            desc.ownerMemberIndex = memberIndex;

            const std::string& memberName = memberNameIt->second;
            auto existing = reflection->uniforms.find(memberName);
            if (existing == reflection->uniforms.end() && reflection->ambiguousUniformNames.find(memberName) == reflection->ambiguousUniformNames.end()) {
                reflection->uniforms[memberName] = desc;
            } else {
                reflection->uniforms.erase(memberName);
                reflection->ambiguousUniformNames.insert(memberName);
            }

            if (!blockName.empty()) {
                reflection->uniforms[blockName + "." + memberName] = desc;
            }
        }
    }

    return reflection;
}

static void ensureUniformWriteRange(std::vector<Uint8>& bytes, size_t offset, size_t size) {
    if (offset + size > bytes.size()) {
        bytes.resize(offset + size, 0);
    }
}

template <typename T>
static void writeUniformScalarBytes(std::vector<Uint8>& bytes, size_t offset, T value) {
    ensureUniformWriteRange(bytes, offset, sizeof(T));
    std::memcpy(bytes.data() + offset, &value, sizeof(T));
}

static Uint16 floatToHalfBits(float value) {
    Uint32 bits;
    std::memcpy(&bits, &value, sizeof(bits));

    const Uint32 sign = (bits >> 16) & 0x8000u;
    Sint32 exponent = (Sint32) ((bits >> 23) & 0xFFu) - 127 + 15;
    Uint32 mantissa = bits & 0x7FFFFFu;

    if (exponent <= 0) {
        if (exponent < -10) {
            return (Uint16) sign;
        }
        mantissa = (mantissa | 0x800000u) >> (1 - exponent);
        return (Uint16) (sign | ((mantissa + 0x1000u) >> 13));
    }
    if (exponent >= 31) {
        return (Uint16) (sign | 0x7C00u | (mantissa ? 0x0200u : 0));
    }

    return (Uint16) (sign | ((Uint32) exponent << 10) | ((mantissa + 0x1000u) >> 13));
}

static bool pushVectorLuaComponent(lua_State* L, int tableIndex, Uint32 component) {
    if (tableIndex < 0) {
        tableIndex = lua_gettop(L) + tableIndex + 1;
    }
    lua_rawgeti(L, tableIndex, (int) component + 1);
    if (!lua_isnil(L, -1)) {
        return true;
    }
    lua_pop(L, 1);

    static const char* fields[] = {"x", "y", "z", "w"};
    if (component < 4) {
        lua_getfield(L, tableIndex, fields[component]);
        return !lua_isnil(L, -1);
    }
    return false;
}

static void writeReflectedUniformValue(lua_State* L, int valueIndex, const ShaderUniformReflection& reflection, Uint32 typeId, const SpirvMemberDecoration* containingMember, size_t destinationOffset, std::vector<Uint8>& bytes);

static void writeReflectedScalar(lua_State* L, int valueIndex, const SpirvUniformType& type, size_t destinationOffset, std::vector<Uint8>& bytes) {
    if (type.kind == SpirvUniformTypeKind::Bool) {
        luaL_checktype(L, valueIndex, LUA_TBOOLEAN);
        writeUniformScalarBytes<Uint32>(bytes, destinationOffset, lua_toboolean(L, valueIndex) ? 1u : 0u);
        return;
    }

    if (type.kind == SpirvUniformTypeKind::Float) {
        const double number = luaL_checknumber(L, valueIndex);
        if (type.width == 16) {
            writeUniformScalarBytes<Uint16>(bytes, destinationOffset, floatToHalfBits((float) number));
        } else if (type.width == 32) {
            writeUniformScalarBytes<float>(bytes, destinationOffset, (float) number);
        } else if (type.width == 64) {
            writeUniformScalarBytes<double>(bytes, destinationOffset, number);
        } else {
            luaL_error(L, "Unsupported shader float width: %u bits.", type.width);
        }
        return;
    }

    const lua_Integer integer = luaL_checkinteger(L, valueIndex);
    if (type.isSigned) {
        if (type.width == 8) {
            if (integer < std::numeric_limits<Sint8>::min() || integer > std::numeric_limits<Sint8>::max()) luaL_error(L, "Uniform value is outside int8 range.");
            writeUniformScalarBytes<Sint8>(bytes, destinationOffset, (Sint8) integer);
        } else if (type.width == 16) {
            if (integer < std::numeric_limits<Sint16>::min() || integer > std::numeric_limits<Sint16>::max()) luaL_error(L, "Uniform value is outside int16 range.");
            writeUniformScalarBytes<Sint16>(bytes, destinationOffset, (Sint16) integer);
        } else if (type.width == 32) {
            if (integer < std::numeric_limits<Sint32>::min() || integer > std::numeric_limits<Sint32>::max()) luaL_error(L, "Uniform value is outside int32 range.");
            writeUniformScalarBytes<Sint32>(bytes, destinationOffset, (Sint32) integer);
        } else if (type.width == 64) {
            writeUniformScalarBytes<Sint64>(bytes, destinationOffset, (Sint64) integer);
        } else {
            luaL_error(L, "Unsupported shader integer width: %u bits.", type.width);
        }
    } else {
        if (integer < 0) {
            luaL_error(L, "Unsigned shader uniform cannot be negative.");
            return;
        }
        const Uint64 value = (Uint64) integer;
        if (type.width == 8) {
            if (value > std::numeric_limits<Sint8>::max()) luaL_error(L, "Uniform value is outside uint8 range.");
            writeUniformScalarBytes<Uint8>(bytes, destinationOffset, (Uint8) value);
        } else if (type.width == 16) {
            if (value > std::numeric_limits<Sint16>::max()) luaL_error(L, "Uniform value is outside uint16 range.");
            writeUniformScalarBytes<Uint16>(bytes, destinationOffset, (Uint16) value);
        } else if (type.width == 32) {
            if (value > std::numeric_limits<Sint32>::max()) luaL_error(L, "Uniform value is outside uint32 range.");
            writeUniformScalarBytes<Uint32>(bytes, destinationOffset, (Uint32) value);
        } else if (type.width == 64) {
            writeUniformScalarBytes<Uint64>(bytes, destinationOffset, value);
        } else {
            luaL_error(L, "Unsupported shader integer width: %u bits.", type.width);
        }
    }
}

static void writeReflectedUniformValue(lua_State* L, int valueIndex, const ShaderUniformReflection& reflection, Uint32 typeId, const SpirvMemberDecoration* containingMember, size_t destinationOffset, std::vector<Uint8>& bytes) {
    auto typeIt = reflection.types.find(typeId);
    if (typeIt == reflection.types.end()) {
        luaL_error(L, "Shader uniform has an unknown reflected type.");
        return;
    }
    const SpirvUniformType& type = typeIt->second;

    if (type.kind == SpirvUniformTypeKind::Bool ||
        type.kind == SpirvUniformTypeKind::Int ||
        type.kind == SpirvUniformTypeKind::Float) {
        writeReflectedScalar(L, valueIndex, type, destinationOffset, bytes);
        return;
    }

    if (type.kind == SpirvUniformTypeKind::Vector) {
        luaL_checktype(L, valueIndex, LUA_TTABLE);
        const size_t componentSize = reflectedTypeSize(reflection, type.elementType);
        for (Uint32 component = 0; component < type.count; ++component) {
            if (!pushVectorLuaComponent(L, valueIndex, component)) {
                luaL_error(L, "Shader vector uniform is missing component %u.", component + 1);
                return;
            }
            writeReflectedUniformValue(
                L, -1, reflection, type.elementType, nullptr,
                destinationOffset + componentSize * component, bytes
            );
            lua_pop(L, 1);
        }
        return;
    }

    if (type.kind == SpirvUniformTypeKind::Matrix) {
        luaL_checktype(L, valueIndex, LUA_TTABLE);
        auto columnIt = reflection.types.find(type.elementType);
        if (columnIt == reflection.types.end() || columnIt->second.kind != SpirvUniformTypeKind::Vector) {
            luaL_error(L, "Shader matrix reflection is invalid.");
            return;
        }
        auto scalarIt = reflection.types.find(columnIt->second.elementType);
        if (scalarIt == reflection.types.end()) {
            luaL_error(L, "Shader matrix scalar reflection is invalid.");
            return;
        }

        const Uint32 rows = columnIt->second.count;
        const Uint32 columns = type.count;
        const size_t scalarSize = uniformScalarSize(scalarIt->second);
        const size_t stride = (containingMember && containingMember->hasMatrixStride)
            ? containingMember->matrixStride
            : 16;
        const bool rowMajor = containingMember && containingMember->rowMajor;

        Uint32 element = 1;
        for (Uint32 column = 0; column < columns; ++column) {
            for (Uint32 row = 0; row < rows; ++row) {
                lua_rawgeti(L, valueIndex, element++);
                if (lua_isnil(L, -1)) {
                    luaL_error(L, "Shader matrix uniform is missing element %u.", element - 1);
                    return;
                }
                const size_t physicalOffset = rowMajor
                    ? destinationOffset + row * stride + column * scalarSize
                    : destinationOffset + column * stride + row * scalarSize;
                writeReflectedUniformValue(L, -1, reflection, columnIt->second.elementType, nullptr, physicalOffset, bytes);
                lua_pop(L, 1);
            }
        }
        return;
    }

    if (type.kind == SpirvUniformTypeKind::Array) {
        luaL_checktype(L, valueIndex, LUA_TTABLE);
        auto lengthIt = reflection.constants.find(type.lengthId);
        auto strideIt = reflection.arrayStrides.find(typeId);
        if (lengthIt == reflection.constants.end() || strideIt == reflection.arrayStrides.end()) {
            luaL_error(L, "Shader array reflection is incomplete.");
            return;
        }

        for (Uint32 i = 0; i < (Uint32) lengthIt->second; ++i) {
            lua_rawgeti(L, valueIndex, i + 1);
            if (lua_isnil(L, -1)) {
                luaL_error(L, "Shader array uniform is missing element %u.", i + 1);
                return;
            }
            writeReflectedUniformValue(
                L, -1, reflection, type.elementType, containingMember,
                destinationOffset + (size_t) strideIt->second * i, bytes
            );
            lua_pop(L, 1);
        }
        return;
    }

    if (type.kind == SpirvUniformTypeKind::Struct) {
        luaL_checktype(L, valueIndex, LUA_TTABLE);
        for (Uint32 memberIndex = 0; memberIndex < type.memberTypes.size(); ++memberIndex) {
            const Uint64 key = spirvMemberKey(typeId, memberIndex);
            auto decorationIt = reflection.memberDecorations.find(key);
            if (decorationIt == reflection.memberDecorations.end() || !decorationIt->second.hasOffset) {
                continue;
            }

            bool hasValue = false;
            auto nameIt = reflection.memberNames.find(key);
            if (nameIt != reflection.memberNames.end() && !nameIt->second.empty()) {
                lua_getfield(L, valueIndex, nameIt->second.c_str());
                hasValue = !lua_isnil(L, -1);
                if (!hasValue) lua_pop(L, 1);
            }
            if (!hasValue) {
                lua_rawgeti(L, valueIndex, memberIndex + 1);
                hasValue = !lua_isnil(L, -1);
            }

            if (hasValue) {
                writeReflectedUniformValue(
                    L, -1, reflection, type.memberTypes[memberIndex], &decorationIt->second,
                    destinationOffset + decorationIt->second.offset, bytes
                );
            }
            lua_pop(L, 1);
        }
        return;
    }

    luaL_error(L, "Unsupported reflected shader uniform type.");
}

int setShaderUniform(lua_State *L) {
    const int shaderId = (int) luaL_checkinteger(L, 1);
    const char* uniformName = luaL_checkstring(L, 2);
    luaL_checkany(L, 3);

    shaderData& data = createdShaders[shaderId];
    if (!data.uniformReflection) {
        luaL_error(L, "Shader %d has no uniform reflection data.", shaderId);
        return 0;
    }

    auto uniformIt = data.uniformReflection->uniforms.find(uniformName);
    if (uniformIt == data.uniformReflection->uniforms.end()) {
        if (data.uniformReflection->ambiguousUniformNames.find(uniformName) != data.uniformReflection->ambiguousUniformNames.end()) {
            luaL_error(L, "Shader uniform '%s' is ambiguous; use 'BufferName.%s'.", uniformName, uniformName);
        } else {
            luaL_error(L, "Shader uniform '%s' does not exist.", uniformName);
        }
        return 0;
    }

    const ShaderUniformDesc& uniform = uniformIt->second;
    auto bufferSizeIt = data.uniformReflection->bufferSizes.find(uniform.slot);
    if (bufferSizeIt == data.uniformReflection->bufferSizes.end() || bufferSizeIt->second == 0) {
        luaL_error(L, "Shader uniform '%s' has an invalid uniform buffer.", uniformName);
        return 0;
    }

    std::vector<Uint8>& bytes = data.uniformBuffers[uniform.slot];
    if (bytes.empty()) {
        bytes.resize(bufferSizeIt->second, 0);
    }

    const SpirvMemberDecoration* memberDecoration = nullptr;
    auto decorationIt = data.uniformReflection->memberDecorations.find(spirvMemberKey(uniform.ownerStructType, uniform.ownerMemberIndex));
    if (decorationIt != data.uniformReflection->memberDecorations.end()) {
        memberDecoration = &decorationIt->second;
    }

    writeReflectedUniformValue(L, 3, *data.uniformReflection, uniform.typeId, memberDecoration, uniform.offset, bytes);

    if (!SDL_SetGPURenderStateFragmentUniforms(data.renderState, uniform.slot, bytes.data(), (Uint32) bytes.size())) {
        luaL_error(L, "Could not set fragment shader uniform '%s': %s", uniformName, SDL_GetError());
        return 0;
    }

    return 0;
}

static const luaL_Reg graphics_lib[] = {
    {"setCursorVisibility", setCursorVisibility},
    {"isCursorVisible", isCursorVisible},
    {"setVsync", setVsync},
    {"isVsyncEnabled", isVsyncEnabled},
    {"setDrawColor", setDrawColor},
    {"clear", clear},
    {"drawRect", drawRect},
    {"drawCircle", drawCircle},
    {"loadImage", loadImage},
    {"setTextureScaleMode", setTextureScaleMode},
    {"unloadImage", unloadImage},
    {"drawImage", drawImage},
    {"drawImageRegion", drawImageRegion},
    {"loadFont", loadFont},
    {"unloadFont", unloadFont},
    {"drawText", drawText},
    {"pushCoord", pushCoord},
    {"translateCoord", translateCoord},
    {"scaleCoord", scaleCoord},
    {"rotateCoord", rotateCoord},
    {"shearCoord", shearCoord},
    {"popCoord", popCoord},
    {"setScissor", setScissor},
    {"getScissor", getScissor},
    {"createShader", createShader},
    {"destroyShader", destroyShader},
    {"setShader", setShader},
    {"setShaderUniform", setShaderUniform},
    {nullptr, nullptr},
};

void registerGraphicsFunctions(lua_State* L, ResourceState* state) {
    TTF_Init();
    resourceState = state;

    renderer = SDL_CreateGPURenderer(nullptr, state->getWindow());
    if (renderer == nullptr) {
        printf("Couldn't create the renderer: %s", SDL_GetError());
    };
    state->setRenderer(renderer);

    luaL_register(L, "graphics", graphics_lib);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "graphics");
}

void updateFontTextCache() {
    for (auto it = textCache.begin(); it != textCache.end();) {
        if (CURRENT_FRAME - it->second.lastFrameUsed >= it->second.lifetime) {
            SDL_DestroyTexture(it->second.texture);
            it = textCache.erase(it);
        } else {
            ++it;
        }
    }
}