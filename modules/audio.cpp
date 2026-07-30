#include "audio.hpp"

#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"
#include "../utils/Pool.hpp"
#include "../dependencies/luau/VM/include/lualib.h"

static ResourceState* resourceState = nullptr;
MIX_Mixer* mixer;
std::unordered_map<int, MIX_Audio*> loadedAudio;
Uint32 audioIDCounter = 0;
Pool<MIX_Track*> tracks([]{
    return MIX_CreateTrack(mixer);
});
std::unordered_map<int, MIX_Track*> playingTracks;
Uint32 trackIDCounter = 0;

int loadAudio(lua_State *L) {
    const char* path = lua_tostring(L, 1);
    std::filesystem::path finalPath = resourceState->getMainPath() / path;
    MIX_Audio* audio = MIX_LoadAudio(mixer, finalPath.c_str(), false);
    if (audio == nullptr) {
        luaL_error(L, "Could not load audio: %s", finalPath.c_str());
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
    for (auto it = playingTracks.begin(); it != playingTracks.end(); ++it) {
        if (it->second == track) {
            playingTracks.erase(it);
            break;
        }
    }
    tracks.free(track);
}

int play(lua_State *L) {
    MIX_Track* track = tracks.get();
    MIX_SetTrackAudio(track, loadedAudio[lua_tonumber(L, 1)]);
    MIX_PlayTrack(track, 0);
    MIX_SetTrackStoppedCallback(track, TrackStoppedCallback, nullptr);

    playingTracks[trackIDCounter] = track;

    lua_pushnumber(L, trackIDCounter);

    trackIDCounter++;

    return 1;
}

int stop(lua_State *L) {
    int index = lua_tointeger(L, 1);
    MIX_StopTrack(playingTracks[index], 0);
    playingTracks.erase(index);
    return 0;
}

int setVolume(lua_State *L) {
    MIX_SetTrackGain(playingTracks[lua_tointeger(L, 1)], lua_tonumber(L, 2));
    return 0;
}

int setPanning(lua_State *L) {
    const MIX_StereoGains gains {.left = lua_tonumber(L, 2), .right = lua_tonumber(L, 3)};
    MIX_SetTrackStereo(playingTracks[lua_tointeger(L, 1)], &gains);
    return 0;
}

int setMasterVolume(lua_State *L) {
    MIX_SetMixerGain(mixer, lua_tonumber(L, 1));
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
    lua_pushcfunction(L, play, "play");
    lua_setfield(L, -2, "play");
    lua_pushcfunction(L, stop, "stop");
    lua_setfield(L, -2, "stop");
    lua_pushcfunction(L, setVolume, "setVolume");
    lua_setfield(L, -2, "setVolume");
    lua_pushcfunction(L, setPanning, "setPanning");
    lua_setfield(L, -2, "setPanning");
    lua_pushcfunction(L, setMasterVolume, "setMasterVolume");
    lua_setfield(L, -2, "setMasterVolume");

    lua_setreadonly(L, -1, 1);

    lua_setfield(L, -2, "audio");
}