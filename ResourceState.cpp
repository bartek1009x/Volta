#include "ResourceState.hpp"

#include "dependencies/luau/VM/include/lua.h"
#include "dependencies/luau/VM/include/lualib.h"
#include <SDL3/SDL_render.h>

ResourceState::ResourceState() : L(luaL_newstate()) {
    luaL_openlibs(L);
}
ResourceState::~ResourceState() {
    lua_close(L);
    if (window != nullptr) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

lua_State* ResourceState::getL() const {
    return L;
}

SDL_Renderer* ResourceState::getRenderer() const {
    return renderer;
}

std::filesystem::path ResourceState::getMainPath() const {
    return mainPath;
}

void ResourceState::setMainPath(std::filesystem::path path) {
    mainPath = path;
}

void ResourceState::setWinRen(SDL_Window* win, SDL_Renderer* ren) {
    window = win;
    renderer = ren;
}