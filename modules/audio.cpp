#include "audio.hpp"

#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"
#include "../utils/Pool.hpp"

static ResourceState* resourceState = nullptr;
MIX_Mixer* mixer;
std::unordered_map<int, MIX_Audio*> loadedAudio;
Uint32 audioIDCounter = 0;
Pool<MIX_Track*> tracks([]{
    return MIX_CreateTrack(mixer);
});

int loadAudio(lua_State *L) {
    const char* path = lua_tostring(L, 1);
    std::filesystem::path finalPath = resourceState->getMainPath() / path;
    MIX_Audio* audio = MIX_LoadAudio(mixer, finalPath.c_str(), false);
    if (audio == nullptr) {
        lua_pushfstring(L, "Could not load audio: %s", finalPath.c_str());
        lua_error(L);
        return 0;
    }

    loadedAudio[audioIDCounter] = audio;

    lua_pushnumber(L, audioIDCounter);

    audioIDCounter++;

    return 1;
}

int unloadAudio(lua_State *L) {
    int index = lua_tonumber(L, 1);
    MIX_DestroyAudio(loadedAudio[index]);
    loadedAudio.erase(index);

    return 0;
}

void SDLCALL TrackStoppedCallback(void *userdata, MIX_Track *track) {
    tracks.free(track);
}

int playAudio(lua_State *L) {
    MIX_Track* track = tracks.get();
    MIX_SetTrackAudio(track, loadedAudio[lua_tonumber(L, 1)]);
    MIX_PlayTrack(track, 0);
    MIX_SetTrackStoppedCallback(track, TrackStoppedCallback, nullptr);
    return 0;
}

void registerAudioFunctions(ResourceState* state) {
    resourceState = state;

    MIX_Init();
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    lua_State* L = state->getL();

    lua_createtable(L, 1, 0);

    lua_pushcfunction(L, loadAudio, "loadAudio");
    lua_setfield(L, -2, "loadAudio");
    lua_pushcfunction(L, unloadAudio, "unloadAudio");
    lua_setfield(L, -2, "unloadAudio");
    lua_pushcfunction(L, playAudio, "playAudio");
    lua_setfield(L, -2, "playAudio");

    lua_setglobal(L, "audio");
}