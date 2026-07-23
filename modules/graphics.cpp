#include "graphics.hpp"
#include <SDL3/SDL_render.h>

SDL_Renderer *renderer = nullptr;

int setDrawColor(lua_State *L) {
    SDL_SetRenderDrawColor(renderer, lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    return 1;
}

int clear(lua_State *L) {
    SDL_RenderClear(renderer);
    return 1;
}

void registerGraphicsFunctions(ResourceState* state) {
    if (renderer == nullptr) {
        renderer = state->getRenderer();
    }
    lua_State* L = state->getL();

    lua_createtable(L, 1, 0);

    lua_pushcfunction(L, setDrawColor, "setDrawColor");
    lua_setfield(L, -2, "setDrawColor");
    lua_pushcfunction(L, clear, "clear");
    lua_setfield(L, -2, "clear");

    lua_setglobal(L, "graphics");
}