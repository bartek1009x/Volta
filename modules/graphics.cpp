#include "graphics.hpp"

#include <iostream>
#include <filesystem>
#include <unordered_map>

#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

using namespace std;

SDL_Renderer *renderer = nullptr;
std::filesystem::path mainPath;
unordered_map<const char*, SDL_Texture*> loadedTextures;

int setDrawColor(lua_State *L) {
    SDL_SetRenderDrawColor(renderer, lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    return 1;
}

int clear(lua_State *L) {
    SDL_RenderClear(renderer);
    return 1;
}

int loadImage(lua_State *L) {
    const char* path = lua_tostring(L, 1);
    SDL_Texture* texture = IMG_LoadTexture(renderer, (mainPath / path).c_str());
    if (texture == nullptr) {
        return 0;
    }

    loadedTextures[path] = texture;

    return 1;
}

int drawImage(lua_State *L) {
    SDL_RenderTexture(renderer, loadedTextures[lua_tostring(L, 1)], nullptr, nullptr);
    return 1;
}

void registerGraphicsFunctions(ResourceState* state) {
    if (renderer == nullptr) {
        renderer = state->getRenderer();
    }
    mainPath = state->getMainPath();
    lua_State* L = state->getL();

    lua_createtable(L, 1, 0);

    lua_pushcfunction(L, setDrawColor, "setDrawColor");
    lua_setfield(L, -2, "setDrawColor");
    lua_pushcfunction(L, clear, "clear");
    lua_setfield(L, -2, "clear");
    lua_pushcfunction(L, loadImage, "loadImage");
    lua_setfield(L, -2, "loadImage");
    lua_pushcfunction(L, drawImage, "drawImage");
    lua_setfield(L, -2, "drawImage");

    lua_setglobal(L, "graphics");
}