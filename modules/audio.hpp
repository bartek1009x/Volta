#ifndef AUDIO_H
#define AUDIO_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int loadAudio(lua_State* L);
int unloadAudio(lua_State* L);
int getDuration(lua_State* L);
int play(lua_State* L);
int stop(lua_State* L);
int pause(lua_State* L);
int resume(lua_State* L);
int getRemaining(lua_State* L);
int setVolume(lua_State* L);
int setPanning(lua_State* L);
int setMasterVolume(lua_State* L);
void registerAudioFunctions(lua_State* L, ResourceState* state);

#endif