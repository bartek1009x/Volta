#ifndef AUDIO_H
#define AUDIO_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int loadAudio(lua_State* L);
int unloadAudio(lua_State* L);
int playAudio(lua_State* L);
void registerAudioFunctions(ResourceState* state);

#endif