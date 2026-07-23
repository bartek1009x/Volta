#include "graphics.hpp"

#include <iostream>
#include <filesystem>
#include <unordered_map>

#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

using namespace std;

SDL_Renderer *renderer = nullptr;
std::filesystem::path mainPath;
unordered_map<int, SDL_Texture*> loadedTextures;
Uint32 textureIDCounter = 0;

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
    std::filesystem::path finalPath = mainPath / path;
    SDL_Texture* texture = IMG_LoadTexture(renderer, finalPath.c_str());
    if (texture == nullptr) {
        lua_pushfstring(L, "Could not load texture: %s", finalPath.c_str());
        lua_error(L);
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

    return 1;
}

int drawImage(lua_State *L) {
    SDL_RenderTexture(renderer, loadedTextures[lua_tonumber(L, 1)], nullptr, nullptr);
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
    lua_pushcfunction(L, unloadImage, "unloadImage");
    lua_setfield(L, -2, "unloadImage");
    lua_pushcfunction(L, drawImage, "drawImage");
    lua_setfield(L, -2, "drawImage");

    lua_setglobal(L, "graphics");
}