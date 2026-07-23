#ifndef RESOURCE_STATE_H
#define RESOURCE_STATE_H

#include <filesystem>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include "dependencies/luau/VM/include/lua.h"

class ResourceState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    lua_State* L;
    std::filesystem::path mainPath;

    public:
        ResourceState();
        ~ResourceState();

        lua_State* getL() const;
        SDL_Renderer* getRenderer() const;
        std::filesystem::path getMainPath() const;
        void setMainPath(std::filesystem::path);
        void setWinRen(SDL_Window* win, SDL_Renderer* ren);
};

#endif