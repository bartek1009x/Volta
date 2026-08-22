#ifndef BOX2D_BINDINGS_H
#define BOX2D_BINDINGS_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int createWorld(lua_State* L);
int destroyWorld(lua_State* L);
int worldIsValid(lua_State* L);
int worldStep(lua_State* L);
int worldGetBounds(lua_State* L);
int worldEnableSleeping(lua_State* L);
int worldIsSleepingEnabled(lua_State* L);
int worldEnableContinuous(lua_State* L);
int worldIsContinuousEnabled(lua_State* L);
int worldSetRestitutionThreshold(lua_State* L);
int worldGetRestitutionThreshold(lua_State* L);
int worldSetHitEventThreshold(lua_State* L);
int worldGetHitEventThreshold(lua_State* L);
int worldSetGravity(lua_State* L);
int worldGetGravity(lua_State* L);
int worldSetContactTuning(lua_State* L);
int worldSetContactRecycleDistance(lua_State* L);
int worldGetContactRecycleDistance(lua_State* L);
int worldSetMaximumLinearSpeed(lua_State* L);
int worldGetMaximumLinearSpeed(lua_State* L);
int worldEnableWarmStarting(lua_State* L);
int worldIsWarmStartingEnabled(lua_State* L);
int worldGetAwakeBodyCount(lua_State* L);
int worldGetCounters(lua_State* L);
int worldGetMaxCapacity(lua_State* L);
int worldSetWorkerCount(lua_State* L);
int worldGetWorkerCount(lua_State* L);
int worldGetStateHash(lua_State* L);
void registerBox2dFunctions(lua_State* L, ResourceState* state);

#endif