#ifndef RESOURCE_STATE_H
#define RESOURCE_STATE_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include "dependencies/luau/VM/include/lua.h"

class ResourceState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    lua_State* L;

    public:
        ResourceState();
        ~ResourceState();

        lua_State* getL() const;
        SDL_Renderer* getRenderer() const;
        void setWinRen(SDL_Window* win, SDL_Renderer* ren);
};

#endif