#ifndef RESOURCE_STATE_H
#define RESOURCE_STATE_H

#include <SDL3/SDL_gpu.h>
#include <filesystem>
#include <memory>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "../dependencies/luau/VM/include/lua.h"
#include "RequireContext.hpp"

class ResourceState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    lua_State* L;
    std::filesystem::path mainPath;

    public:
        ResourceState();
        ~ResourceState();

        lua_State* getL() const;

        SDL_Window* getWindow() const;
        void setWindow(SDL_Window* win);

        SDL_Renderer* getRenderer() const;
        void setRenderer(SDL_Renderer* renderer);

        std::filesystem::path getMainPath() const;
        void setMainPath(std::filesystem::path);

        std::unique_ptr<RequireContext> requireContext;

        float mouseWheelX, mouseWheelY;
};

#endif
