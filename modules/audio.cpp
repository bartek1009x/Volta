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
    const std::string stringPath = finalPath.string(); // we need to convert it here, because if we don't, path.c_str() will return const wchar_t* on windows (we need const char*)
    const char* pathCStr = stringPath.c_str();
    MIX_Audio* audio = MIX_LoadAudio(mixer, pathCStr, false);
    if (audio == nullptr) {
        luaL_error(L, "Could not load audio: %s", pathCStr);
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

int getDuration(lua_State *L) {
    int index = lua_tonumber(L, 1);

    Sint64 frames = MIX_GetAudioDuration(loadedAudio[index]);
    double seconds = MIX_AudioFramesToMS(loadedAudio[index], frames) / 1000.0;

    lua_pushnumber(L, seconds);

    return 1;
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

int pause(lua_State *L) {
    int index = lua_tointeger(L, 1);
    MIX_PauseTrack(playingTracks[index]);
    return 0;
}

int resume(lua_State *L) {
    int index = lua_tointeger(L, 1);
    MIX_ResumeTrack(playingTracks[index]);
    return 0;
}

int getRemaining(lua_State *L) {
    int index = lua_tonumber(L, 1);

    Sint64 frames = MIX_GetTrackRemaining(playingTracks[index]);
    double seconds = MIX_TrackFramesToMS(playingTracks[index], frames) / 1000.0;

    lua_pushnumber(L, seconds);

    return 1;
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

static const luaL_Reg audio_lib[] = {
    {"loadAudio", loadAudio},
    {"unloadAudio", unloadAudio},
    {"getDuration", getDuration},
    {"play", play},
    {"stop", stop},
    {"pause", pause},
    {"resume", resume},
    {"getRemaining", getRemaining},
    {"setVolume", setVolume},
    {"setPanning", setPanning},
    {"setMasterVolume", setMasterVolume},
    {nullptr, nullptr},
};

void registerAudioFunctions(ResourceState* state) {
    resourceState = state;

    MIX_Init();
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    lua_State* L = state->getL();

    luaL_register(L, "audio", audio_lib);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "audio");
}