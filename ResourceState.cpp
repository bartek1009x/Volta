#include "ResourceState.hpp"

#include "dependencies/luau/VM/include/lua.h"
#include "dependencies/luau/VM/include/lualib.h"

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

void ResourceState::setWinRen(SDL_Window* win, SDL_Renderer* ren) {
    window = win;
    renderer = ren;
}