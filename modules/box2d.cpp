#include "box2d.hpp"

#include "../dependencies/luau/VM/include/lualib.h"
#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"

#include <SDL3/SDL_stdinc.h>

b2WorldDef constructWorldDef(lua_State* L, int defTableIndex) {
    b2WorldDef def = b2DefaultWorldDef();

    lua_pushnil(L);
    while (lua_next(L, defTableIndex) != 0) {
        const char* field = lua_tostring(L, -2);

        if (strcmp(field, "gravity") == 0) {
            int gravityIndex = lua_gettop(L);

            lua_pushnil(L);
            while (lua_next(L, gravityIndex) != 0) {
                const char* gravityField = lua_tostring(L, -2);

                if (strcmp(gravityField, "x") == 0) {
                    def.gravity.x = lua_tonumber(L, -1);
                }
                else if (strcmp(gravityField, "y") == 0) {
                    def.gravity.y = lua_tonumber(L, -1);
                }

                lua_pop(L, 1);
            }
        }
        else if (strcmp(field, "restitutionThreshold") == 0) {
            def.restitutionThreshold = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "hitEventThreshold") == 0) {
            def.hitEventThreshold = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "contactHertz") == 0) {
            def.contactHertz = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "contactDampingRatio") == 0) {
            def.contactDampingRatio = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "contactSpeed") == 0) {
            def.contactSpeed = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "maximumLinearSpeed") == 0) {
            def.maximumLinearSpeed = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "enableSleep") == 0) {
            def.enableSleep = lua_toboolean(L, -1);
        }
        else if (strcmp(field, "enableContinuous") == 0) {
            def.enableContinuous = lua_toboolean(L, -1);
        }
        else if (strcmp(field, "enableContactSoftening") == 0) {
            def.enableContactSoftening = lua_toboolean(L, -1);
        }
        else if (strcmp(field, "workerCount") == 0) {
            def.workerCount = lua_tointeger(L, -1);
        }
        else if (strcmp(field, "capacity") == 0) {
            int capacityIndex = lua_gettop(L);

            lua_pushnil(L);
            while (lua_next(L, capacityIndex) != 0) {
                const char* capacityField = lua_tostring(L, -2);

                if (strcmp(capacityField, "staticShapeCount") == 0) {
                    def.capacity.staticShapeCount = lua_tointeger(L, -1);
                }
                else if (strcmp(capacityField, "dynamicShapeCount") == 0) {
                    def.capacity.dynamicShapeCount = lua_tointeger(L, -1);
                }
                else if (strcmp(capacityField, "staticBodyCount") == 0) {
                    def.capacity.staticBodyCount = lua_tointeger(L, -1);
                }
                else if (strcmp(capacityField, "dynamicBodyCount") == 0) {
                    def.capacity.dynamicBodyCount = lua_tointeger(L, -1);
                }
                else if (strcmp(capacityField, "contactCount") == 0) {
                    def.capacity.contactCount = lua_tointeger(L, -1);
                }

                lua_pop(L, 1);
            }
        }

        lua_pop(L, 1);
    }

    return def;
}

b2WorldId getIdFromLuau(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_argerror(L, 1, "The world ID must be a table");
        return {0,0};
    }
    lua_rawgeti(L, 1, 1);
    Uint16 index1 = lua_tointeger(L, -1);
    lua_rawgeti(L, 1, 2);
    Uint16 generation = lua_tointeger(L, -1);
    b2WorldId id = {index1, generation};
    return id;
}

int createWorld(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_argerror(L, 1, "The world definition must be a table");
        return 0;
    }
    b2WorldDef def = constructWorldDef(L, 1);
    b2WorldId worldId = b2CreateWorld(&def);

    lua_createtable(L, 2, 0);

    lua_pushinteger(L, worldId.index1);
    lua_rawseti(L, -2, 1);

    lua_pushinteger(L, worldId.generation);
    lua_rawseti(L, -2, 2);

    return 2;
}

int destroyWorld(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    b2DestroyWorld(id);

    return 0;
}

int worldIsValid(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    lua_pushboolean(L, b2World_IsValid(id));

    return 1;
}

int worldStep(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    float timeStep = lua_tonumber(L, 2);
    int subStepCount = lua_tointeger(L, 3);

    b2World_Step(id, timeStep, subStepCount);

    return 0;
}

int worldGetBounds(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);

    b2AABB aabb = b2World_GetBounds(id);
    lua_pushnumber(L, aabb.lowerBound.x);
    lua_pushnumber(L, aabb.lowerBound.y);
    lua_pushnumber(L, aabb.upperBound.x);
    lua_pushnumber(L, aabb.upperBound.y);

    return 4;
}

int worldEnableSleeping(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2World_EnableSleeping(id, flag);

    return 0;
}

int worldIsSleepingEnabled(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    lua_pushboolean(L, b2World_IsSleepingEnabled(id));

    return 1;
}

int worldEnableContinuous(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2World_EnableContinuous(id, flag);

    return 0;
}

int worldIsContinuousEnabled(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    lua_pushboolean(L, b2World_IsContinuousEnabled(id));

    return 1;
}

int worldSetRestitutionThreshold(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    float value = lua_tonumber(L, 2);

    b2World_SetRestitutionThreshold(id, value);

    return 0;
}

int worldGetRestitutionThreshold(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    lua_pushnumber(L, b2World_GetRestitutionThreshold(id));

    return 1;
}

int worldSetHitEventThreshold(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    float value = lua_tonumber(L, 2);

    b2World_SetHitEventThreshold(id, value);

    return 0;
}

int worldGetHitEventThreshold(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    lua_pushnumber(L, b2World_GetHitEventThreshold(id));

    return 1;
}

int worldSetGravity(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);

    b2World_SetGravity(id, {x, y});

    return 0;
}

int worldGetGravity(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);

    b2Vec2 gravity = b2World_GetGravity(id);
    lua_pushnumber(L, gravity.x);
    lua_pushnumber(L, gravity.y);

    return 2;
}

int worldSetContactTuning(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);
    float dampingRatio = lua_tonumber(L, 3);
    float pushSpeed = lua_tonumber(L, 4);

    b2World_SetContactTuning(id, hertz, dampingRatio, pushSpeed);

    return 0;
}

int worldSetContactRecycleDistance(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    float recycleDistance = lua_tonumber(L, 2);

    b2World_SetContactRecycleDistance(id, recycleDistance);

    return 0;
}

int worldGetContactRecycleDistance(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);

    lua_pushnumber(L, b2World_GetContactRecycleDistance(id));

    return 1;
}

int worldSetMaximumLinearSpeed(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    float maximumLinearSpeed = lua_tonumber(L, 2);

    b2World_SetMaximumLinearSpeed(id, maximumLinearSpeed);

    return 0;
}

int worldGetMaximumLinearSpeed(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);

    lua_pushnumber(L, b2World_GetMaximumLinearSpeed(id));

    return 1;
}

int worldEnableWarmStarting(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2World_EnableWarmStarting(id, flag);

    return 0;
}

int worldIsWarmStartingEnabled(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);

    lua_pushboolean(L, b2World_IsWarmStartingEnabled(id));

    return 1;
}

int worldGetAwakeBodyCount(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);

    lua_pushinteger(L, b2World_GetAwakeBodyCount(id));

    return 1;
}

int worldGetMaxCapacity(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    b2Capacity capacity = b2World_GetMaxCapacity(id);

    lua_createtable(L, 0, 5);

    lua_pushinteger(L, capacity.staticShapeCount);
    lua_setfield(L, -2, "staticShapeCount");

    lua_pushinteger(L, capacity.dynamicShapeCount);
    lua_setfield(L, -2, "dynamicShapeCount");

    lua_pushinteger(L, capacity.staticBodyCount);
    lua_setfield(L, -2, "staticBodyCount");

    lua_pushinteger(L, capacity.dynamicBodyCount);
    lua_setfield(L, -2, "dynamicBodyCount");

    lua_pushinteger(L, capacity.contactCount);
    lua_setfield(L, -2, "contactCount");

    return 1;
}

int worldSetWorkerCount(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    int count = lua_tointeger(L, 2);

    b2World_SetWorkerCount(id, count);

    return 0;
}

int worldGetWorkerCount(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);

    lua_pushinteger(L, b2World_GetWorkerCount(id));

    return 1;
}

int worldGetStateHash(lua_State* L) {
    b2WorldId id = getIdFromLuau(L);
    uint64_t hash = b2World_GetStateHash(id);

    lua_pushinteger64(L, static_cast<int64_t>(hash));

    return 1;
}

static const luaL_Reg box2d_lib[] = {
    {"createWorld", createWorld},
    {"destroyWorld", destroyWorld},
    {"worldIsValid", worldIsValid},
    {"worldStep", worldStep},
    {"worldGetBounds", worldGetBounds},
    {"worldEnableSleeping", worldEnableSleeping},
    {"worldIsSleepingEnabled", worldIsSleepingEnabled},
    {"worldEnableContinuous", worldEnableContinuous},
    {"worldIsContinuousEnabled", worldIsContinuousEnabled},
    {"worldSetRestitutionThreshold", worldSetRestitutionThreshold},
    {"worldGetRestitutionThreshold", worldGetRestitutionThreshold},
    {"worldSetHitEventThreshold", worldSetHitEventThreshold},
    {"worldGetHitEventThreshold", worldGetHitEventThreshold},
    {"worldSetGravity", worldSetGravity},
    {"worldGetGravity", worldGetGravity},
    {"worldSetContactTuning", worldSetContactTuning},
    {"worldSetContactRecycleDistance", worldSetContactRecycleDistance},
    {"worldGetContactRecycleDistance", worldGetContactRecycleDistance},
    {"worldSetMaximumLinearSpeed", worldSetMaximumLinearSpeed},
    {"worldGetMaximumLinearSpeed", worldGetMaximumLinearSpeed},
    {"worldEnableWarmStarting", worldEnableWarmStarting},
    {"worldIsWarmStartingEnabled", worldIsWarmStartingEnabled},
    {"worldGetAwakeBodyCount", worldGetAwakeBodyCount},
    {"worldGetMaxCapacity", worldGetMaxCapacity},
    {"worldSetWorkerCount", worldSetWorkerCount},
    {"worldGetWorkerCount", worldGetWorkerCount},
    {"worldGetStateHash", worldGetStateHash},
    {nullptr, nullptr},
};

void registerBox2dFunctions(lua_State* L, ResourceState* state) {
    luaL_register(L, "box2d", box2d_lib);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "box2d");
}