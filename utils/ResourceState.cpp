#include "ResourceState.hpp"

#include "../dependencies/luau/VM/include/lualib.h"
#include "../dependencies/luau/CodeGen/include/Luau/CodeGen.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

ResourceState::ResourceState() : L(luaL_newstate()) {
    if (Luau::CodeGen::isSupported()) {
        Luau::CodeGen::create(L);
    }
    luaL_openlibs(L);
}
ResourceState::~ResourceState() {
    lua_close(L);
    if (window != nullptr) {
        SDL_DestroyGPUDevice(SDL_GetGPURendererDevice(renderer));
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
    TTF_Quit();
    MIX_Quit();
    SDL_Quit();
}

lua_State* ResourceState::getL() const {
    return L;
}

SDL_Window* ResourceState::getWindow() const {
    return window;
}

void ResourceState::setWindow(SDL_Window* win) {
    window = win;
}

SDL_Renderer* ResourceState::getRenderer() const {
    return renderer;
}

void ResourceState::setRenderer(SDL_Renderer* ren) {
    renderer = ren;
}

std::filesystem::path ResourceState::getMainPath() const {
    return mainPath;
}

void ResourceState::setMainPath(std::filesystem::path path) {
    mainPath = path;
}