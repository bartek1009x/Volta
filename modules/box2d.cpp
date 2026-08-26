#include "box2d.hpp"

#include "../dependencies/luau/VM/include/lualib.h"
#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/id.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"

#include <SDL3/SDL_stdinc.h>

// World

b2WorldDef constructWorldDef(lua_State* L, int defTableIndex) {
    b2WorldDef def = b2DefaultWorldDef();

    lua_getfield(L, defTableIndex, "gravity");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), def.gravity);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "restitutionThreshold");
    if (!lua_isnil(L, -1)) {
        def.restitutionThreshold = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "hitEventThreshold");
    if (!lua_isnil(L, -1)) {
        def.hitEventThreshold = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "contactHertz");
    if (!lua_isnil(L, -1)) {
        def.contactHertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "contactDampingRatio");
    if (!lua_isnil(L, -1)) {
        def.contactDampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "contactSpeed");
    if (!lua_isnil(L, -1)) {
        def.contactSpeed = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "maximumLinearSpeed");
    if (!lua_isnil(L, -1)) {
        def.maximumLinearSpeed = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableSleep");
    if (!lua_isnil(L, -1)) {
        def.enableSleep = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableContinuous");
    if (!lua_isnil(L, -1)) {
        def.enableContinuous = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableContactSoftening");
    if (!lua_isnil(L, -1)) {
        def.enableContactSoftening = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "workerCount");
    if (!lua_isnil(L, -1)) {
        def.workerCount = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "capacity");
    if (lua_istable(L, -1)) {
        int capacityIndex = lua_gettop(L);

        lua_getfield(L, capacityIndex, "staticShapeCount");
        if (!lua_isnil(L, -1)) {
            def.capacity.staticShapeCount = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, capacityIndex, "dynamicShapeCount");
        if (!lua_isnil(L, -1)) {
            def.capacity.dynamicShapeCount = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, capacityIndex, "staticBodyCount");
        if (!lua_isnil(L, -1)) {
            def.capacity.staticBodyCount = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, capacityIndex, "dynamicBodyCount");
        if (!lua_isnil(L, -1)) {
            def.capacity.dynamicBodyCount = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, capacityIndex, "contactCount");
        if (!lua_isnil(L, -1)) {
            def.capacity.contactCount = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return def;
}

b2ExplosionDef constructExplosionDef(lua_State* L, int index) {
    b2ExplosionDef def = b2DefaultExplosionDef();

    lua_getfield(L, index, "maskBits");
    if (!lua_isnil(L, -1)) {
        def.maskBits = lua_tointeger64(L, -1, nullptr);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "position");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), def.position);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "radius");
    if (!lua_isnil(L, -1)) {
        def.radius = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "falloff");
    if (!lua_isnil(L, -1)) {
        def.falloff = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "impulsePerLength");
    if (!lua_isnil(L, -1)) {
        def.impulsePerLength = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return def;
}

b2WorldId getWorldIdFromLuau(lua_State* L) {
    if (lua_type(L, 1) != LUA_TINTEGER) {
        luaL_argerror(L, 1, "The world ID must be an integer");
        return {};
    }

    Uint32 storedId = lua_tointeger64(L, 1, nullptr);
    return b2LoadWorldId(storedId);
}

void pushWorldId(lua_State* L, b2WorldId id) {
    lua_pushinteger64(L, b2StoreWorldId(id));
}

void pushContactId(lua_State* L, b2ContactId id) {
    Uint32 values[3];
    b2StoreContactId(id, values);

    lua_pushinteger64(L, values[0]);
    lua_pushinteger64(L, values[1]);
    lua_pushinteger64(L, values[2]);
}

int createWorld(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_argerror(L, 1, "The world definition must be a table");
        return 0;
    }
    b2WorldDef def = constructWorldDef(L, 1);
    b2WorldId worldId = b2CreateWorld(&def);

    pushWorldId(L, worldId);

    return 1;
}

int destroyWorld(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    b2DestroyWorld(id);

    return 0;
}

int worldIsValid(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    lua_pushboolean(L, b2World_IsValid(id));

    return 1;
}

int worldStep(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    float timeStep = lua_tonumber(L, 2);
    int subStepCount = lua_tointeger(L, 3);

    b2World_Step(id, timeStep, subStepCount);

    return 0;
}

int worldGetBounds(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2AABB aabb = b2World_GetBounds(id);
    lua_pushnumber(L, aabb.lowerBound.x);
    lua_pushnumber(L, aabb.lowerBound.y);
    lua_pushnumber(L, aabb.upperBound.x);
    lua_pushnumber(L, aabb.upperBound.y);

    return 4;
}

int worldGetBodyEvents(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    b2BodyEvents events = b2World_GetBodyEvents(id);

    lua_createtable(L, events.moveCount, 0);

    for (int i = 0; i < events.moveCount; ++i) {
        const b2BodyMoveEvent& event = events.moveEvents[i];

        lua_createtable(L, 0, 6);

        lua_pushnumber(L, event.transform.p.x);
        lua_setfield(L, -2, "x");

        lua_pushnumber(L, event.transform.p.y);
        lua_setfield(L, -2, "y");

        lua_pushnumber(L, event.transform.q.c);
        lua_setfield(L, -2, "c");

        lua_pushnumber(L, event.transform.q.s);
        lua_setfield(L, -2, "s");

        pushBodyId(L, event.bodyId);
        lua_setfield(L, -2, "bodyId");

        lua_pushboolean(L, event.fellAsleep);
        lua_setfield(L, -2, "fellAsleep");

        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

int worldGetContactEvents(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    b2ContactEvents events = b2World_GetContactEvents(id);

    lua_createtable(L, 0, 6);

    // Begin events
    lua_createtable(L, events.beginCount, 0);

    for (int i = 0; i < events.beginCount; ++i) {
        const b2ContactBeginTouchEvent& event = events.beginEvents[i];

        lua_createtable(L, 0, 5);

        pushShapeId(L, event.shapeIdA);
        lua_setfield(L, -2, "shapeIdA");

        pushShapeId(L, event.shapeIdB);
        lua_setfield(L, -2, "shapeIdB");

        pushContactId(L, event.contactId);
        lua_setfield(L, -4, "contactIdGeneration");
        lua_setfield(L, -3, "contactIdWorld0");
        lua_setfield(L, -2, "contactIdIndex1");

        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "beginEvents");

    // End events
    lua_createtable(L, events.endCount, 0);

    for (int i = 0; i < events.endCount; ++i) {
        const b2ContactEndTouchEvent& event = events.endEvents[i];

        lua_createtable(L, 0, 5);

        pushShapeId(L, event.shapeIdA);
        lua_setfield(L, -2, "shapeIdA");

        pushShapeId(L, event.shapeIdB);
        lua_setfield(L, -2, "shapeIdB");

        pushContactId(L, event.contactId);
        lua_setfield(L, -4, "contactIdGeneration");
        lua_setfield(L, -3, "contactIdWorld0");
        lua_setfield(L, -2, "contactIdIndex1");

        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "endEvents");

    // Hit events
    lua_createtable(L, events.hitCount, 0);

    for (int i = 0; i < events.hitCount; ++i) {
        const b2ContactHitEvent& event = events.hitEvents[i];

        lua_createtable(L, 0, 8);

        pushShapeId(L, event.shapeIdA);
        lua_setfield(L, -2, "shapeIdA");

        pushShapeId(L, event.shapeIdB);
        lua_setfield(L, -2, "shapeIdB");

        pushContactId(L, event.contactId);
        lua_setfield(L, -4, "contactIdGeneration");
        lua_setfield(L, -3, "contactIdWorld0");
        lua_setfield(L, -2, "contactIdIndex1");

        pushVec2(L, event.point);
        lua_setfield(L, -2, "point");

        pushVec2(L, event.normal);
        lua_setfield(L, -2, "normal");

        lua_pushnumber(L, event.approachSpeed);
        lua_setfield(L, -2, "approachSpeed");

        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "hitEvents");

    lua_pushinteger(L, events.beginCount);
    lua_setfield(L, -2, "beginCount");

    lua_pushinteger(L, events.endCount);
    lua_setfield(L, -2, "endCount");

    lua_pushinteger(L, events.hitCount);
    lua_setfield(L, -2, "hitCount");

    return 1;
}

int worldGetSensorEvents(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    b2SensorEvents events = b2World_GetSensorEvents(id);

    lua_createtable(L, 0, 4);

    lua_createtable(L, events.beginCount, 0);

    for (int i = 0; i < events.beginCount; ++i) {
        const b2SensorBeginTouchEvent& event = events.beginEvents[i];

        lua_createtable(L, 0, 2);

        pushShapeId(L, event.sensorShapeId);
        lua_setfield(L, -2, "sensorShapeId");

        pushShapeId(L, event.visitorShapeId);
        lua_setfield(L, -2, "visitorShapeId");

        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "beginEvents");

    lua_createtable(L, events.endCount, 0);

    for (int i = 0; i < events.endCount; ++i) {
        const b2SensorEndTouchEvent& event = events.endEvents[i];

        lua_createtable(L, 0, 2);

        pushShapeId(L, event.sensorShapeId);
        lua_setfield(L, -2, "sensorShapeId");

        pushShapeId(L, event.visitorShapeId);
        lua_setfield(L, -2, "visitorShapeId");

        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "endEvents");

    lua_pushinteger(L, events.beginCount);
    lua_setfield(L, -2, "beginCount");

    lua_pushinteger(L, events.endCount);
    lua_setfield(L, -2, "endCount");

    return 1;
}

struct OverlapResultContext {
    lua_State* L;
    int callbackIndex;
};

bool overlapResultCallback(b2ShapeId shapeId, void* context) {
    OverlapResultContext* overlapContext = static_cast<OverlapResultContext*>(context);
    lua_State* L = overlapContext->L;

    lua_pushvalue(L, overlapContext->callbackIndex);

    pushShapeId(L, shapeId);

    lua_call(L, 1, 1);

    bool result = lua_toboolean(L, -1);
    lua_pop(L, 1);

    return result;
}

int worldOverlapAABB(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2Pos origin = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2AABB aabb = {
        {
            (float) (lua_tonumber(L, 4)),
            (float) (lua_tonumber(L, 5))
        },
        {
            (float) (lua_tonumber(L, 6)),
            (float) (lua_tonumber(L, 7))
        }
    };

    b2QueryFilter filter = constructQueryFilter(L, 8);

    if (lua_type(L, 9) != LUA_TFUNCTION) {
        luaL_argerror(L, 9, "The overlap callback must be a function");
        return 0;
    }

    OverlapResultContext context = {L, 9};

    b2TreeStats stats = b2World_OverlapAABB(id, origin, aabb, filter, overlapResultCallback, &context);

    pushTreeStats(L, stats);

    return 1;
}

int worldOverlapShape(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2Pos origin = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    if (!lua_istable(L, 4)) {
        luaL_argerror(L, 4, "The shape proxy must be a table");
        return 0;
    }

    b2ShapeProxy proxy = constructShapeProxy(L, 4);
    b2QueryFilter filter = constructQueryFilter(L, 5);

    if (lua_type(L, 6) != LUA_TFUNCTION) {
        luaL_argerror(L, 6, "The overlap callback must be a function");
        return 0;
    }

    OverlapResultContext context = {
        L,
        6
    };

    b2TreeStats stats = b2World_OverlapShape(id, origin, &proxy, filter, overlapResultCallback, &context);

    pushTreeStats(L, stats);

    return 1;
}

int worldEnableSleeping(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2World_EnableSleeping(id, flag);

    return 0;
}

int worldIsSleepingEnabled(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    lua_pushboolean(L, b2World_IsSleepingEnabled(id));

    return 1;
}

int worldEnableContinuous(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2World_EnableContinuous(id, flag);

    return 0;
}

int worldIsContinuousEnabled(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    lua_pushboolean(L, b2World_IsContinuousEnabled(id));

    return 1;
}

int worldSetRestitutionThreshold(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    float value = lua_tonumber(L, 2);

    b2World_SetRestitutionThreshold(id, value);

    return 0;
}

int worldGetRestitutionThreshold(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    lua_pushnumber(L, b2World_GetRestitutionThreshold(id));

    return 1;
}

int worldSetHitEventThreshold(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    float value = lua_tonumber(L, 2);

    b2World_SetHitEventThreshold(id, value);

    return 0;
}

int worldGetHitEventThreshold(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    lua_pushnumber(L, b2World_GetHitEventThreshold(id));

    return 1;
}

int worldSetGravity(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);

    b2World_SetGravity(id, {x, y});

    return 0;
}

int worldGetGravity(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2Vec2 gravity = b2World_GetGravity(id);
    lua_pushnumber(L, gravity.x);
    lua_pushnumber(L, gravity.y);

    return 2;
}

int worldExplode(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The explosion definition must be a table");
        return 0;
    }

    b2ExplosionDef def = constructExplosionDef(L, 2);

    b2World_Explode(id, &def);

    return 0;
}

int worldSetContactTuning(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);
    float dampingRatio = lua_tonumber(L, 3);
    float pushSpeed = lua_tonumber(L, 4);

    b2World_SetContactTuning(id, hertz, dampingRatio, pushSpeed);

    return 0;
}

int worldSetContactRecycleDistance(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    float recycleDistance = lua_tonumber(L, 2);

    b2World_SetContactRecycleDistance(id, recycleDistance);

    return 0;
}

int worldGetContactRecycleDistance(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    lua_pushnumber(L, b2World_GetContactRecycleDistance(id));

    return 1;
}

int worldSetMaximumLinearSpeed(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    float maximumLinearSpeed = lua_tonumber(L, 2);

    b2World_SetMaximumLinearSpeed(id, maximumLinearSpeed);

    return 0;
}

int worldGetMaximumLinearSpeed(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    lua_pushnumber(L, b2World_GetMaximumLinearSpeed(id));

    return 1;
}

int worldEnableWarmStarting(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2World_EnableWarmStarting(id, flag);

    return 0;
}

int worldIsWarmStartingEnabled(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    lua_pushboolean(L, b2World_IsWarmStartingEnabled(id));

    return 1;
}

int worldGetAwakeBodyCount(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    lua_pushinteger(L, b2World_GetAwakeBodyCount(id));

    return 1;
}

int worldGetMaxCapacity(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
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

int worldGetCounters(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    b2Counters counters = b2World_GetCounters(id);

    lua_createtable(L, 0, 13);

    lua_pushinteger64(L, counters.byteCount);
    lua_setfield(L, -2, "byteCount");

    lua_pushinteger(L, counters.bodyCount);
    lua_setfield(L, -2, "bodyCount");

    lua_pushinteger(L, counters.shapeCount);
    lua_setfield(L, -2, "shapeCount");

    lua_pushinteger(L, counters.contactCount);
    lua_setfield(L, -2, "contactCount");

    lua_pushinteger(L, counters.jointCount);
    lua_setfield(L, -2, "jointCount");

    lua_pushinteger(L, counters.islandCount);
    lua_setfield(L, -2, "islandCount");

    lua_pushinteger(L, counters.stackUsed);
    lua_setfield(L, -2, "stackUsed");

    lua_pushinteger(L, counters.staticTreeHeight);
    lua_setfield(L, -2, "staticTreeHeight");

    lua_pushinteger(L, counters.treeHeight);
    lua_setfield(L, -2, "treeHeight");

    lua_pushinteger(L, counters.taskCount);
    lua_setfield(L, -2, "taskCount");

    lua_createtable(L, 24, 0);
    for (int i = 0; i < 24; ++i) {
        lua_pushinteger(L, counters.colorCounts[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "colorCounts");

    lua_pushinteger(L, counters.awakeContactCount);
    lua_setfield(L, -2, "awakeContactCount");

    lua_pushinteger(L, counters.recycledContactCount);
    lua_setfield(L, -2, "recycledContactCount");

    return 1;
}

int worldSetWorkerCount(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    int count = lua_tointeger(L, 2);

    b2World_SetWorkerCount(id, count);

    return 0;
}

int worldGetWorkerCount(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    lua_pushinteger(L, b2World_GetWorkerCount(id));

    return 1;
}

int worldGetStateHash(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    Uint64 hash = b2World_GetStateHash(id);

    lua_pushinteger64(L, hash);

    return 1;
}

b2ContactId getContactIdFromLuau(lua_State* L) {
    for (int i = 1; i <= 3; ++i) {
        if (lua_type(L, i) != LUA_TINTEGER) {
            luaL_argerror(L, i, "The contact ID must be three integers: index1, world0, generation");
            return {};
        }
    }

    Uint32 values[3] = {
        (Uint32) (lua_tointeger64(L, 1, nullptr)),
        (Uint32) (lua_tointeger64(L, 2, nullptr)),
        (Uint32) (lua_tointeger64(L, 3, nullptr))
    };
    return b2LoadContactId(values);
}

int contactIsValid(lua_State* L) {
    b2ContactId id = getContactIdFromLuau(L);

    lua_pushboolean(L, b2Contact_IsValid(id));

    return 1;
}

// Rayast

b2QueryFilter constructQueryFilter(lua_State* L, int index) {
    b2QueryFilter filter = b2DefaultQueryFilter();

    if (!lua_istable(L, index)) {
        luaL_argerror(L, index, "The query filter must be a table");
        return filter;
    }

    lua_getfield(L, index, "categoryBits");
    if (!lua_isnil(L, -1)) {
        filter.categoryBits = (Uint64) (lua_tointeger64(L, -1, nullptr));
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maskBits");
    if (!lua_isnil(L, -1)) {
        filter.maskBits = (Uint64) (lua_tointeger64(L, -1, nullptr));
    }
    lua_pop(L, 1);

    return filter;
}

void pushTreeStats(lua_State* L, b2TreeStats stats) {
    lua_createtable(L, 0, 2);

    lua_pushinteger(L, stats.nodeVisits);
    lua_setfield(L, -2, "nodeVisits");

    lua_pushinteger(L, stats.leafVisits);
    lua_setfield(L, -2, "leafVisits");
}

void pushRayResult(lua_State* L, b2RayResult result) {
    lua_createtable(L, 0, result.hit ? 7 : 3);

    lua_pushboolean(L, result.hit);
    lua_setfield(L, -2, "hit");

    lua_pushinteger(L, result.nodeVisits);
    lua_setfield(L, -2, "nodeVisits");

    lua_pushinteger(L, result.leafVisits);
    lua_setfield(L, -2, "leafVisits");

    if (result.hit) {
        pushShapeId(L, result.shapeId);
        lua_setfield(L, -2, "shapeId");

        pushVec2(L, result.point);
        lua_setfield(L, -2, "point");

        pushVec2(L, result.normal);
        lua_setfield(L, -2, "normal");

        lua_pushnumber(L, result.fraction);
        lua_setfield(L, -2, "fraction");
    }
}

struct CastResultContext {
    lua_State* L;
    int callbackIndex;
};

float castResultCallback(b2ShapeId shapeId, b2Pos point, b2Vec2 normal, float fraction, void* context) {
    CastResultContext* castContext = static_cast<CastResultContext*>(context);
    lua_State* L = castContext->L;

    lua_pushvalue(L, castContext->callbackIndex);

    pushShapeId(L, shapeId);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    lua_pushnumber(L, normal.x);
    lua_pushnumber(L, normal.y);

    lua_pushnumber(L, fraction);

    lua_call(L, 6, 1);

    float result = lua_tonumber(L, -1);
    lua_pop(L, 1);

    return result;
}

int worldCastRay(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2Pos origin = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Vec2 translation = {
        (float) (lua_tonumber(L, 4)),
        (float) (lua_tonumber(L, 5))
    };

    b2QueryFilter filter = constructQueryFilter(L, 6);

    if (lua_type(L, 7) != LUA_TFUNCTION) {
        luaL_argerror(L, 7, "The cast callback must be a function");
        return 0;
    }

    CastResultContext context = {L, 7};

    b2TreeStats stats = b2World_CastRay(id, origin, translation, filter, castResultCallback, &context);

    pushTreeStats(L, stats);

    return 1;
}

int worldCastRayClosest(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2Pos origin = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Vec2 translation = {
        (float) (lua_tonumber(L, 4)),
        (float) (lua_tonumber(L, 5))
    };

    b2QueryFilter filter = constructQueryFilter(L, 6);

    b2RayResult result = b2World_CastRayClosest(id, origin, translation, filter);

    pushRayResult(L, result);

    return 1;
}

b2ShapeProxy constructShapeProxy(lua_State* L, int index) {
    b2ShapeProxy proxy = {};

    lua_getfield(L, index, "count");
    if (!lua_isnil(L, -1)) {
        proxy.count = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    if (proxy.count < 0) {
        proxy.count = 0;
    }
    else if (proxy.count > B2_MAX_POLYGON_VERTICES) {
        proxy.count = B2_MAX_POLYGON_VERTICES;
    }

    lua_getfield(L, index, "points");
    if (lua_istable(L, -1)) {
        int pointsIndex = lua_gettop(L);

        for (int i = 0; i < proxy.count; ++i) {
            lua_rawgeti(L, pointsIndex, i + 1);

            if (lua_istable(L, -1)) {
                applyVec2(L, lua_gettop(L), proxy.points[i]);
            }

            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "radius");
    if (!lua_isnil(L, -1)) {
        proxy.radius = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return proxy;
}

int worldCastShape(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2Pos origin = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    if (!lua_istable(L, 4)) {
        luaL_argerror(L, 4, "The shape proxy must be a table");
        return 0;
    }

    b2ShapeProxy proxy = constructShapeProxy(L, 4);

    b2Vec2 translation = {
        (float) (lua_tonumber(L, 5)),
        (float) (lua_tonumber(L, 6))
    };

    b2QueryFilter filter = constructQueryFilter(L, 7);

    if (lua_type(L, 8) != LUA_TFUNCTION) {
        luaL_argerror(L, 8, "The cast callback must be a function");
        return 0;
    }

    CastResultContext context = {L, 8};

    b2TreeStats stats = b2World_CastShape(id, origin, &proxy, translation, filter, castResultCallback, &context);

    pushTreeStats(L, stats);

    return 1;
}

int worldCastMover(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2Pos origin = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    if (!lua_istable(L, 4)) {
        luaL_argerror(L, 4, "The mover capsule must be a table");
        return 0;
    }

    b2Capsule mover = constructCapsule(L, 4);

    b2Vec2 translation = {
        (float) (lua_tonumber(L, 5)),
        (float) (lua_tonumber(L, 6))
    };

    b2QueryFilter filter = constructQueryFilter(L, 7);

    float fraction = b2World_CastMover(id, origin, &mover, translation, filter);

    lua_pushnumber(L, fraction);

    return 1;
}

struct PlaneResultContext {
    lua_State* L;
    int callbackIndex;
};

void pushPlane(lua_State* L, b2Plane plane) {
    lua_createtable(L, 0, 2);

    pushVec2(L, plane.normal);
    lua_setfield(L, -2, "normal");

    lua_pushnumber(L, plane.offset);
    lua_setfield(L, -2, "offset");
}

void pushPlaneResult(lua_State* L, const b2PlaneResult& result) {
    lua_createtable(L, 0, 3);

    pushPlane(L, result.plane);
    lua_setfield(L, -2, "plane");

    pushVec2(L, result.point);
    lua_setfield(L, -2, "point");

    lua_pushboolean(L, result.hit);
    lua_setfield(L, -2, "hit");
}

bool planeResultCallback(b2ShapeId shapeId, const b2PlaneResult* plane, void* context) {
    PlaneResultContext* planeContext = static_cast<PlaneResultContext*>(context);
    lua_State* L = planeContext->L;

    lua_pushvalue(L, planeContext->callbackIndex);

    pushShapeId(L, shapeId);
    pushPlaneResult(L, *plane);

    lua_call(L, 2, 1);

    bool result = lua_toboolean(L, -1);
    lua_pop(L, 1);

    return result;
}

int worldCollideMover(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);

    b2Pos origin = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    if (!lua_istable(L, 4)) {
        luaL_argerror(L, 4, "The mover capsule must be a table");
        return 0;
    }

    b2Capsule mover = constructCapsule(L, 4);
    b2QueryFilter filter = constructQueryFilter(L, 5);

    if (lua_type(L, 6) != LUA_TFUNCTION) {
        luaL_argerror(L, 6, "The plane callback must be a function");
        return 0;
    }

    PlaneResultContext context = {L, 6};

    b2World_CollideMover(id, origin, &mover, filter, planeResultCallback, &context);

    return 0;
}

// Body

b2BodyId getBodyIdFromLuau(lua_State* L) {
    if (lua_type(L, 1) != LUA_TINTEGER) {
        luaL_argerror(L, 1, "The body ID must be an integer");
        return {};
    }

    Uint64 storedId = lua_tointeger64(L, 1, nullptr);
    return b2LoadBodyId(storedId);
}

void pushBodyId(lua_State* L, b2BodyId id) {
    lua_pushinteger64(L, b2StoreBodyId(id));
}

b2MassData getMassDataFromLuau(lua_State* L, int index) {
    if (lua_type(L, index) != LUA_TTABLE) {
        luaL_argerror(L, index, "Mass data must be a table");
        return {};
    }

    b2MassData data = {};

    lua_getfield(L, index, "mass");
    data.mass = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "center");
    if (lua_type(L, -1) == LUA_TTABLE) {
        int centerIndex = lua_gettop(L);

        lua_getfield(L, centerIndex, "x");
        data.center.x = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, centerIndex, "y");
        data.center.y = lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "rotationalInertia");
    data.rotationalInertia = lua_tonumber(L, -1);
    lua_pop(L, 1);

    return data;
}

void pushMassData(lua_State* L, b2MassData data) {
    lua_createtable(L, 0, 3);

    lua_pushnumber(L, data.mass);
    lua_setfield(L, -2, "mass");

    lua_createtable(L, 0, 2);

    lua_pushnumber(L, data.center.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, data.center.y);
    lua_setfield(L, -2, "y");

    lua_setfield(L, -2, "center");

    lua_pushnumber(L, data.rotationalInertia);
    lua_setfield(L, -2, "rotationalInertia");
}

b2MotionLocks getMotionLocksFromLuau(lua_State* L, int index) {
    if (lua_type(L, index) != LUA_TTABLE) {
        luaL_argerror(L, index, "Motion locks must be a table");
        return {};
    }

    b2MotionLocks locks = {};

    lua_getfield(L, index, "linearX");
    locks.linearX = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "linearY");
    locks.linearY = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "angularZ");
    locks.angularZ = lua_toboolean(L, -1);
    lua_pop(L, 1);

    return locks;
}

void pushMotionLocks(lua_State* L, b2MotionLocks locks) {
    lua_createtable(L, 0, 3);

    lua_pushboolean(L, locks.linearX);
    lua_setfield(L, -2, "linearX");

    lua_pushboolean(L, locks.linearY);
    lua_setfield(L, -2, "linearY");

    lua_pushboolean(L, locks.angularZ);
    lua_setfield(L, -2, "angularZ");
}

b2BodyDef constructBodyDef(lua_State* L, int defTableIndex) {
    b2BodyDef def = b2DefaultBodyDef();

    lua_getfield(L, defTableIndex, "type");
    if (!lua_isnil(L, -1)) {
        def.type = static_cast<b2BodyType>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "position");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), def.position);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "rotation");
    if (lua_istable(L, -1)) {
        int rotationIndex = lua_gettop(L);

        lua_getfield(L, rotationIndex, "c");
        if (!lua_isnil(L, -1)) {
            def.rotation.c = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, rotationIndex, "s");
        if (!lua_isnil(L, -1)) {
            def.rotation.s = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "linearVelocity");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), def.linearVelocity);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "angularVelocity");
    if (!lua_isnil(L, -1)) {
        def.angularVelocity = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "linearDamping");
    if (!lua_isnil(L, -1)) {
        def.linearDamping = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "angularDamping");
    if (!lua_isnil(L, -1)) {
        def.angularDamping = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "gravityScale");
    if (!lua_isnil(L, -1)) {
        def.gravityScale = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "sleepThreshold");
    if (!lua_isnil(L, -1)) {
        def.sleepThreshold = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "safetyFactor");
    if (!lua_isnil(L, -1)) {
        def.safetyFactor = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "name");
    if (!lua_isnil(L, -1)) {
        def.name = lua_tostring(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "motionLocks");
    if (lua_istable(L, -1)) {
        int motionLocksIndex = lua_gettop(L);

        lua_getfield(L, motionLocksIndex, "linearX");
        if (!lua_isnil(L, -1)) {
            def.motionLocks.linearX = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, motionLocksIndex, "linearY");
        if (!lua_isnil(L, -1)) {
            def.motionLocks.linearY = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, motionLocksIndex, "angularZ");
        if (!lua_isnil(L, -1)) {
            def.motionLocks.angularZ = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableSleep");
    if (!lua_isnil(L, -1)) {
        def.enableSleep = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "isAwake");
    if (!lua_isnil(L, -1)) {
        def.isAwake = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "isBullet");
    if (!lua_isnil(L, -1)) {
        def.isBullet = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "isEnabled");
    if (!lua_isnil(L, -1)) {
        def.isEnabled = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "allowFastRotation");
    if (!lua_isnil(L, -1)) {
        def.allowFastRotation = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableContactRecycling");
    if (!lua_isnil(L, -1)) {
        def.enableContactRecycling = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    return def;
}

int createBody(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (lua_type(L, 2) != LUA_TTABLE) {
        luaL_argerror(L, 2, "The body definition must be a table");
        return 0;
    }

    b2BodyDef def = constructBodyDef(L, 2);
    b2BodyId bodyId = b2CreateBody(worldId, &def);

    pushBodyId(L, bodyId);

    return 1;
}

int destroyBody(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2DestroyBody(id);

    return 0;
}

int bodyIsValid(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushboolean(L, b2Body_IsValid(id));

    return 1;
}

int bodyGetType(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushinteger(L, b2Body_GetType(id));

    return 1;
}

int bodySetType(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2BodyType type = static_cast<b2BodyType>(lua_tointeger(L, 2));

    b2Body_SetType(id, type);

    return 0;
}

int bodySetName(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    const char* name = lua_tostring(L, 2);

    b2Body_SetName(id, name);

    return 0;
}

int bodyGetName(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    const char* name = b2Body_GetName(id);

    if (name != nullptr) {
        lua_pushstring(L, name);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

int bodyGetPosition(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2Pos position = b2Body_GetPosition(id);

    lua_pushnumber(L, position.x);
    lua_pushnumber(L, position.y);

    return 2;
}

int bodyGetRotation(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2Rot rotation = b2Body_GetRotation(id);

    lua_pushnumber(L, rotation.c);
    lua_pushnumber(L, rotation.s);

    return 2;
}

int bodyGetTransform(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2WorldTransform transform = b2Body_GetTransform(id);

    lua_pushnumber(L, transform.p.x);
    lua_pushnumber(L, transform.p.y);
    lua_pushnumber(L, transform.q.c);
    lua_pushnumber(L, transform.q.s);

    return 4;
}

int bodySetTransform(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Pos position = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Rot rotation = {
        (float) (lua_tonumber(L, 4)),
        (float) (lua_tonumber(L, 5))
    };

    b2Body_SetTransform(id, position, rotation);

    return 0;
}

int bodyGetLocalPoint(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Pos worldPoint = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Vec2 point = b2Body_GetLocalPoint(id, worldPoint);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int bodyGetWorldPoint(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 localPoint = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Pos point = b2Body_GetWorldPoint(id, localPoint);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int bodyGetLocalVector(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 worldVector = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Vec2 vector = b2Body_GetLocalVector(id, worldVector);

    lua_pushnumber(L, vector.x);
    lua_pushnumber(L, vector.y);

    return 2;
}

int bodyGetWorldVector(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 localVector = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Vec2 vector = b2Body_GetWorldVector(id, localVector);

    lua_pushnumber(L, vector.x);
    lua_pushnumber(L, vector.y);

    return 2;
}

int bodyGetLinearVelocity(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2Vec2 velocity = b2Body_GetLinearVelocity(id);

    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);

    return 2;
}

int bodyGetAngularVelocity(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushnumber(L, b2Body_GetAngularVelocity(id));

    return 1;
}

int bodySetLinearVelocity(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 velocity = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Body_SetLinearVelocity(id, velocity);

    return 0;
}

int bodySetAngularVelocity(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    float velocity = lua_tonumber(L, 2);

    b2Body_SetAngularVelocity(id, velocity);

    return 0;
}

int bodySetTargetTransform(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2WorldTransform target = {};

    target.p.x = lua_tonumber(L, 2);
    target.p.y = lua_tonumber(L, 3);

    target.q.c = lua_tonumber(L, 4);
    target.q.s = lua_tonumber(L, 5);

    float timeStep = lua_tonumber(L, 6);
    bool wake = lua_toboolean(L, 7);

    b2Body_SetTargetTransform(id, target, timeStep, wake);

    return 0;
}

int bodyGetLocalPointVelocity(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 localPoint = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Vec2 velocity = b2Body_GetLocalPointVelocity(id, localPoint);

    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);

    return 2;
}

int bodyGetWorldPointVelocity(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Pos worldPoint = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Vec2 velocity = b2Body_GetWorldPointVelocity(id, worldPoint);

    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);

    return 2;
}

int bodyApplyForce(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 force = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Pos point = {
        (float) (lua_tonumber(L, 4)),
        (float) (lua_tonumber(L, 5))
    };

    bool wake = lua_toboolean(L, 6);

    b2Body_ApplyForce(id, force, point, wake);

    return 0;
}

int bodyApplyForceToCenter(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 force = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    bool wake = lua_toboolean(L, 4);

    b2Body_ApplyForceToCenter(id, force, wake);

    return 0;
}

int bodyApplyTorque(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    float torque = lua_tonumber(L, 2);
    bool wake = lua_toboolean(L, 3);

    b2Body_ApplyTorque(id, torque, wake);

    return 0;
}

int bodyClearForces(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Body_ClearForces(id);

    return 0;
}

int bodyApplyLinearImpulse(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 impulse = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Pos point = {
        (float) (lua_tonumber(L, 4)),
        (float) (lua_tonumber(L, 5))
    };

    bool wake = lua_toboolean(L, 6);

    b2Body_ApplyLinearImpulse(id, impulse, point, wake);

    return 0;
}

int bodyApplyLinearImpulseToCenter(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 impulse = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    bool wake = lua_toboolean(L, 4);

    b2Body_ApplyLinearImpulseToCenter(id, impulse, wake);

    return 0;
}

int bodyApplyAngularImpulse(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    float impulse = lua_tonumber(L, 2);
    bool wake = lua_toboolean(L, 3);

    b2Body_ApplyAngularImpulse(id, impulse, wake);

    return 0;
}

int bodyGetMass(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushnumber(L, b2Body_GetMass(id));

    return 1;
}

int bodyGetRotationalInertia(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushnumber(L, b2Body_GetRotationalInertia(id));

    return 1;
}

int bodyGetLocalCenter(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2Vec2 center = b2Body_GetLocalCenter(id);

    lua_pushnumber(L, center.x);
    lua_pushnumber(L, center.y);

    return 2;
}

int bodyGetWorldCenter(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2Pos center = b2Body_GetWorldCenter(id);

    lua_pushnumber(L, center.x);
    lua_pushnumber(L, center.y);

    return 2;
}

int bodySetMassData(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2MassData massData = getMassDataFromLuau(L, 2);

    b2Body_SetMassData(id, massData);

    return 0;
}

int bodyGetMassData(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2MassData massData = b2Body_GetMassData(id);

    pushMassData(L, massData);

    return 1;
}

int bodyApplyMassFromShapes(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Body_ApplyMassFromShapes(id);

    return 0;
}

int bodySetLinearDamping(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    float value = lua_tonumber(L, 2);

    b2Body_SetLinearDamping(id, value);

    return 0;
}

int bodyGetLinearDamping(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushnumber(L, b2Body_GetLinearDamping(id));

    return 1;
}

int bodySetAngularDamping(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    float value = lua_tonumber(L, 2);

    b2Body_SetAngularDamping(id, value);

    return 0;
}

int bodyGetAngularDamping(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushnumber(L, b2Body_GetAngularDamping(id));

    return 1;
}

int bodySetGravityScale(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    float value = lua_tonumber(L, 2);

    b2Body_SetGravityScale(id, value);

    return 0;
}

int bodyGetGravityScale(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushnumber(L, b2Body_GetGravityScale(id));

    return 1;
}

int bodyIsAwake(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushboolean(L, b2Body_IsAwake(id));

    return 1;
}

int bodySetAwake(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    bool awake = lua_toboolean(L, 2);

    b2Body_SetAwake(id, awake);

    return 0;
}

int bodyWakeTouching(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Body_WakeTouching(id);

    return 0;
}

int bodyEnableSleep(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    bool enableSleep = lua_toboolean(L, 2);

    b2Body_EnableSleep(id, enableSleep);

    return 0;
}

int bodyIsSleepEnabled(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushboolean(L, b2Body_IsSleepEnabled(id));

    return 1;
}

int bodySetSleepThreshold(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    float threshold = lua_tonumber(L, 2);

    b2Body_SetSleepThreshold(id, threshold);

    return 0;
}

int bodyGetSleepThreshold(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushnumber(L, b2Body_GetSleepThreshold(id));

    return 1;
}

int bodySetSafetyFactor(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    float safetyFactor = lua_tonumber(L, 2);

    b2Body_SetSafetyFactor(id, safetyFactor);

    return 0;
}

int bodyGetSafetyFactor(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushnumber(L, b2Body_GetSafetyFactor(id));

    return 1;
}

int bodyIsEnabled(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushboolean(L, b2Body_IsEnabled(id));

    return 1;
}

int bodyDisable(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Body_Disable(id);

    return 0;
}

int bodyEnable(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Body_Enable(id);

    return 0;
}

int bodySetMotionLocks(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2MotionLocks locks = getMotionLocksFromLuau(L, 2);

    b2Body_SetMotionLocks(id, locks);

    return 0;
}

int bodyGetMotionLocks(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2MotionLocks locks = b2Body_GetMotionLocks(id);

    pushMotionLocks(L, locks);

    return 1;
}

int bodySetBullet(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2Body_SetBullet(id, flag);

    return 0;
}

int bodyIsBullet(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushboolean(L, b2Body_IsBullet(id));

    return 1;
}

int bodyEnableContactRecycling(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2Body_EnableContactRecycling(id, flag);

    return 0;
}

int bodyIsContactRecyclingEnabled(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushboolean(L, b2Body_IsContactRecyclingEnabled(id));

    return 1;
}

int bodyEnableContactEvents(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2Body_EnableContactEvents(id, flag);

    return 0;
}

int bodyEnableHitEvents(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2Body_EnableHitEvents(id, flag);

    return 0;
}

int bodyGetWorld(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);
    b2WorldId worldId = b2Body_GetWorld(id);

    pushWorldId(L, worldId);

    return 1;
}

int bodyGetShapeCount(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushinteger(L, b2Body_GetShapeCount(id));

    return 1;
}

int bodyGetShapes(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    int capacity = b2Body_GetShapeCount(id);

    if (capacity <= 0) {
        lua_createtable(L, 0, 0);
        return 1;
    }

    b2ShapeId stackShapes[8];
    b2ShapeId* shapes = (capacity <= 8) ? stackShapes : new b2ShapeId[capacity];
    int count = b2Body_GetShapes(id, shapes, capacity);

    lua_createtable(L, count, 0);

    for (int i = 0; i < count; ++i) {
        pushShapeId(L, shapes[i]);
        lua_rawseti(L, -2, i + 1);
    }

    if (shapes != stackShapes) delete[] shapes;

    return 1;
}

int bodyGetJointCount(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushinteger(L, b2Body_GetJointCount(id));

    return 1;
}

int bodyGetJoints(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    int capacity = b2Body_GetJointCount(id);

    if (capacity <= 0) {
        lua_createtable(L, 0, 0);
        return 1;
    }

    b2JointId stackJoints[8];
    b2JointId* joints = (capacity <= 8) ? stackJoints : new b2JointId[capacity];
    int count = b2Body_GetJoints(id, joints, capacity);

    lua_createtable(L, count, 0);

    for (int i = 0; i < count; ++i) {
        pushJointId(L, joints[i]);
        lua_rawseti(L, -2, i + 1);
    }

    if (joints != stackJoints) delete[] joints;

    return 1;
}

int bodyGetContactCapacity(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushinteger(L, b2Body_GetContactCapacity(id));

    return 1;
}

int bodyComputeAABB(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2AABB aabb = b2Body_ComputeAABB(id);

    lua_pushnumber(L, aabb.lowerBound.x);
    lua_pushnumber(L, aabb.lowerBound.y);
    lua_pushnumber(L, aabb.upperBound.x);
    lua_pushnumber(L, aabb.upperBound.y);

    return 4;
}

// Shape

b2ShapeId getShapeIdFromLuau(lua_State* L) {
    if (lua_type(L, 1) != LUA_TINTEGER) {
        luaL_argerror(L, 1, "The shape ID must be an integer");
        return {};
    }

    Uint64 storedId = lua_tointeger64(L, 1, nullptr);
    return b2LoadShapeId(storedId);
}

void pushShapeId(lua_State* L, b2ShapeId id) {
    lua_pushinteger64(L, b2StoreShapeId(id));
}

void applyVec2(lua_State* L, int index, b2Vec2& value) {
    if (lua_type(L, index) != LUA_TTABLE) {
        return;
    }

    lua_getfield(L, index, "x");
    if (!lua_isnil(L, -1)) {
        value.x = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "y");
    if (!lua_isnil(L, -1)) {
        value.y = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
}

void pushVec2(lua_State* L, b2Vec2 value) {
    lua_createtable(L, 0, 2);

    lua_pushnumber(L, value.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, value.y);
    lua_setfield(L, -2, "y");
}

void applySurfaceMaterial(lua_State* L, int index, b2SurfaceMaterial& material) {
    if (lua_type(L, index) != LUA_TTABLE) {
        return;
    }

    lua_getfield(L, index, "friction");
    if (!lua_isnil(L, -1)) {
        material.friction = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "restitution");
    if (!lua_isnil(L, -1)) {
        material.restitution = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "rollingResistance");
    if (!lua_isnil(L, -1)) {
        material.rollingResistance = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "tangentSpeed");
    if (!lua_isnil(L, -1)) {
        material.tangentSpeed = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "userMaterialId");
    if (!lua_isnil(L, -1)) {
        material.userMaterialId = lua_tointeger64(L, -1, nullptr);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "customColor");
    if (!lua_isnil(L, -1)) {
        material.customColor = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
}

void pushSurfaceMaterial(lua_State* L, b2SurfaceMaterial material) {
    lua_createtable(L, 0, 6);

    lua_pushnumber(L, material.friction);
    lua_setfield(L, -2, "friction");

    lua_pushnumber(L, material.restitution);
    lua_setfield(L, -2, "restitution");

    lua_pushnumber(L, material.rollingResistance);
    lua_setfield(L, -2, "rollingResistance");

    lua_pushnumber(L, material.tangentSpeed);
    lua_setfield(L, -2, "tangentSpeed");

    lua_pushinteger64(L, material.userMaterialId);
    lua_setfield(L, -2, "userMaterialId");

    lua_pushnumber(L, material.customColor);
    lua_setfield(L, -2, "customColor");
}

b2Segment constructSegment(lua_State* L, int index) {
    b2Segment segment = {};

    lua_getfield(L, index, "point1");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), segment.point1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "point2");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), segment.point2);
    }
    lua_pop(L, 1);

    return segment;
}

void pushSegment(lua_State* L, b2Segment segment) {
    lua_createtable(L, 0, 2);

    pushVec2(L, segment.point1);
    lua_setfield(L, -2, "point1");

    pushVec2(L, segment.point2);
    lua_setfield(L, -2, "point2");
}

b2Filter constructFilter(lua_State* L, int index) {
    b2Filter filter = b2DefaultFilter();

    lua_getfield(L, index, "categoryBits");
    if (!lua_isnil(L, -1)) {
        filter.categoryBits = lua_tointeger64(L, -1, nullptr);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maskBits");
    if (!lua_isnil(L, -1)) {
        filter.maskBits = lua_tointeger64(L, -1, nullptr);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "groupIndex");
    if (!lua_isnil(L, -1)) {
        filter.groupIndex = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    return filter;
}

void applyFilter(lua_State* L, int index, b2Filter& filter) {
    if (!lua_istable(L, index)) {
        return;
    }

    lua_getfield(L, index, "categoryBits");
    if (!lua_isnil(L, -1)) {
        filter.categoryBits = lua_tointeger64(L, -1, nullptr);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maskBits");
    if (!lua_isnil(L, -1)) {
        filter.maskBits = lua_tointeger64(L, -1, nullptr);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "groupIndex");
    if (!lua_isnil(L, -1)) {
        filter.groupIndex = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
}

void pushFilter(lua_State* L, b2Filter filter) {
    lua_createtable(L, 0, 3);

    lua_pushinteger64(L, filter.categoryBits);
    lua_setfield(L, -2, "categoryBits");

    lua_pushinteger64(L, filter.maskBits);
    lua_setfield(L, -2, "maskBits");

    lua_pushinteger(L, filter.groupIndex);
    lua_setfield(L, -2, "groupIndex");
}

b2Circle constructCircle(lua_State* L, int index) {
    b2Circle circle = {};

    lua_getfield(L, index, "center");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), circle.center);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "radius");
    if (!lua_isnil(L, -1)) {
        circle.radius = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return circle;
}

void pushCircle(lua_State* L, b2Circle circle) {
    lua_createtable(L, 0, 2);

    pushVec2(L, circle.center);
    lua_setfield(L, -2, "center");

    lua_pushnumber(L, circle.radius);
    lua_setfield(L, -2, "radius");
}

b2Capsule constructCapsule(lua_State* L, int index) {
    b2Capsule capsule = {};

    lua_getfield(L, index, "center1");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), capsule.center1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "center2");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), capsule.center2);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "radius");
    if (!lua_isnil(L, -1)) {
        capsule.radius = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return capsule;
}

void pushCapsule(lua_State* L, b2Capsule capsule) {
    lua_createtable(L, 0, 3);

    pushVec2(L, capsule.center1);
    lua_setfield(L, -2, "center1");

    pushVec2(L, capsule.center2);
    lua_setfield(L, -2, "center2");

    lua_pushnumber(L, capsule.radius);
    lua_setfield(L, -2, "radius");
}

b2Polygon constructPolygon(lua_State* L, int index) {
    b2Polygon polygon = {};

    lua_getfield(L, index, "count");
    if (!lua_isnil(L, -1)) {
        polygon.count = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    if (polygon.count < 0) {
        polygon.count = 0;
    }
    else if (polygon.count > B2_MAX_POLYGON_VERTICES) {
        polygon.count = B2_MAX_POLYGON_VERTICES;
    }

    lua_getfield(L, index, "vertices");
    if (lua_istable(L, -1)) {
        int verticesIndex = lua_gettop(L);

        for (int i = 0; i < polygon.count; ++i) {
            lua_rawgeti(L, verticesIndex, i + 1);

            if (lua_istable(L, -1)) {
                applyVec2(L, lua_gettop(L), polygon.vertices[i]);
            }

            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "normals");
    if (lua_istable(L, -1)) {
        int normalsIndex = lua_gettop(L);

        for (int i = 0; i < polygon.count; ++i) {
            lua_rawgeti(L, normalsIndex, i + 1);

            if (lua_istable(L, -1)) {
                applyVec2(L, lua_gettop(L), polygon.normals[i]);
            }

            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "centroid");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), polygon.centroid);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "radius");
    if (!lua_isnil(L, -1)) {
        polygon.radius = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return polygon;
}

void pushPolygon(lua_State* L, b2Polygon polygon) {
    lua_createtable(L, 0, 5);

    lua_createtable(L, polygon.count, 0);
    for (int i = 0; i < polygon.count; ++i) {
        pushVec2(L, polygon.vertices[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "vertices");

    lua_createtable(L, polygon.count, 0);
    for (int i = 0; i < polygon.count; ++i) {
        pushVec2(L, polygon.normals[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "normals");

    pushVec2(L, polygon.centroid);
    lua_setfield(L, -2, "centroid");

    lua_pushnumber(L, polygon.radius);
    lua_setfield(L, -2, "radius");

    lua_pushinteger(L, polygon.count);
    lua_setfield(L, -2, "count");
}

b2ShapeDef constructShapeDef(lua_State* L, int defTableIndex) {
    b2ShapeDef def = b2DefaultShapeDef();

    lua_getfield(L, defTableIndex, "material");
    if (lua_istable(L, -1)) {
        applySurfaceMaterial(L, lua_gettop(L), def.material);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "density");
    if (!lua_isnil(L, -1)) {
        def.density = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "filter");
    if (lua_istable(L, -1)) {
        applyFilter(L, lua_gettop(L), def.filter);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableCustomFiltering");
    if (!lua_isnil(L, -1)) {
        def.enableCustomFiltering = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "isSensor");
    if (!lua_isnil(L, -1)) {
        def.isSensor = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableSensorEvents");
    if (!lua_isnil(L, -1)) {
        def.enableSensorEvents = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableContactEvents");
    if (!lua_isnil(L, -1)) {
        def.enableContactEvents = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enableHitEvents");
    if (!lua_isnil(L, -1)) {
        def.enableHitEvents = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "enablePreSolveEvents");
    if (!lua_isnil(L, -1)) {
        def.enablePreSolveEvents = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "invokeContactCreation");
    if (!lua_isnil(L, -1)) {
        def.invokeContactCreation = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, defTableIndex, "updateBodyMass");
    if (!lua_isnil(L, -1)) {
        def.updateBodyMass = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    return def;
}

void pushChainSegment(lua_State* L, b2ChainSegment chainSegment) {
    lua_createtable(L, 0, 3);

    pushVec2(L, chainSegment.ghost1);
    lua_setfield(L, -2, "ghost1");

    pushSegment(L, chainSegment.segment);
    lua_setfield(L, -2, "segment");

    pushVec2(L, chainSegment.ghost2);
    lua_setfield(L, -2, "ghost2");
}

void applySegment(lua_State* L, int index, b2Segment& segment) {
    lua_getfield(L, index, "point1");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), segment.point1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "point2");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), segment.point2);
    }
    lua_pop(L, 1);
}

void applyChainSegment(lua_State* L, int index, b2ChainSegment& chainSegment) {
    lua_getfield(L, index, "ghost1");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), chainSegment.ghost1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "segment");
    if (lua_istable(L, -1)) {
        applySegment(L, lua_gettop(L), chainSegment.segment);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "ghost2");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), chainSegment.ghost2);
    }
    lua_pop(L, 1);
}

b2ChainSegment constructChainSegment(lua_State* L, int index) {
    b2ChainSegment chainSegment = {};
    applyChainSegment(L, index, chainSegment);
    return chainSegment;
}

void pushChainId(lua_State* L, b2ChainId id) {
    lua_pushinteger64(L, b2StoreChainId(id));
}

int createCircleShape(lua_State* L) {
    b2BodyId bodyId = getBodyIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The shape definition must be a table");
        return 0;
    }

    if (!lua_istable(L, 3)) {
        luaL_argerror(L, 3, "The circle must be a table");
        return 0;
    }

    b2ShapeDef def = constructShapeDef(L, 2);
    b2Circle circle = constructCircle(L, 3);

    b2ShapeId id = b2CreateCircleShape(bodyId, &def, &circle);

    pushShapeId(L, id);

    return 1;
}

int createSegmentShape(lua_State* L) {
    b2BodyId bodyId = getBodyIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The shape definition must be a table");
        return 0;
    }

    if (!lua_istable(L, 3)) {
        luaL_argerror(L, 3, "The segment must be a table");
        return 0;
    }

    b2ShapeDef def = constructShapeDef(L, 2);
    b2Segment segment = constructSegment(L, 3);

    b2ShapeId id = b2CreateSegmentShape(bodyId, &def, &segment);

    pushShapeId(L, id);

    return 1;
}

int createCapsuleShape(lua_State* L) {
    b2BodyId bodyId = getBodyIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The shape definition must be a table");
        return 0;
    }

    if (!lua_istable(L, 3)) {
        luaL_argerror(L, 3, "The capsule must be a table");
        return 0;
    }

    b2ShapeDef def = constructShapeDef(L, 2);
    b2Capsule capsule = constructCapsule(L, 3);

    b2ShapeId id = b2CreateCapsuleShape(bodyId, &def, &capsule);

    pushShapeId(L, id);

    return 1;
}

int createPolygonShape(lua_State* L) {
    b2BodyId bodyId = getBodyIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The shape definition must be a table");
        return 0;
    }

    if (!lua_istable(L, 3)) {
        luaL_argerror(L, 3, "The polygon must be a table");
        return 0;
    }

    b2ShapeDef def = constructShapeDef(L, 2);
    b2Polygon polygon = constructPolygon(L, 3);

    b2ShapeId id = b2CreatePolygonShape(bodyId, &def, &polygon);

    pushShapeId(L, id);

    return 1;
}

int destroyShape(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);
    bool updateBodyMass = lua_toboolean(L, 2);

    b2DestroyShape(id, updateBodyMass);

    return 0;
}

int shapeIsValid(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushboolean(L, b2Shape_IsValid(id));

    return 1;
}

int shapeGetType(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushinteger(L, b2Shape_GetType(id));

    return 1;
}

int shapeGetBody(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushBodyId(L, b2Shape_GetBody(id));

    return 1;
}

int shapeGetWorld(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushWorldId(L, b2Shape_GetWorld(id));

    return 1;
}

int shapeIsSensor(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushboolean(L, b2Shape_IsSensor(id));

    return 1;
}

int shapeSetDensity(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);
    float density = lua_tonumber(L, 2);
    bool updateBodyMass = lua_toboolean(L, 3);

    b2Shape_SetDensity(id, density, updateBodyMass);

    return 0;
}

int shapeGetDensity(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushnumber(L, b2Shape_GetDensity(id));

    return 1;
}

int shapeSetFriction(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);
    float friction = lua_tonumber(L, 2);

    b2Shape_SetFriction(id, friction);

    return 0;
}

int shapeGetFriction(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushnumber(L, b2Shape_GetFriction(id));

    return 1;
}

int shapeSetRestitution(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);
    float restitution = lua_tonumber(L, 2);

    b2Shape_SetRestitution(id, restitution);

    return 0;
}

int shapeGetRestitution(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushnumber(L, b2Shape_GetRestitution(id));

    return 1;
}

int shapeSetUserMaterial(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    Uint64 material = lua_tointeger64(L, 2, nullptr);

    b2Shape_SetUserMaterial(id, material);

    return 0;
}

int shapeGetUserMaterial(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    Uint64 material = b2Shape_GetUserMaterial(id);

    lua_pushinteger64(L, material);

    return 1;
}

int shapeSetSurfaceMaterial(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "Surface material must be a table");
        return 0;
    }

    b2SurfaceMaterial material = b2Shape_GetSurfaceMaterial(id);

    applySurfaceMaterial(L, 2, material);

    b2Shape_SetSurfaceMaterial(id, &material);

    return 0;
}

int shapeGetSurfaceMaterial(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushSurfaceMaterial(L, b2Shape_GetSurfaceMaterial(id));

    return 1;
}

int shapeGetFilter(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    b2Filter filter = b2Shape_GetFilter(id);
    pushFilter(L, filter);

    return 1;
}

int shapeSetFilter(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The filter must be a table");
        return 0;
    }

    b2Filter filter = b2Shape_GetFilter(id);
    applyFilter(L, 2, filter);

    b2Shape_SetFilter(id, filter);

    return 0;
}

int shapeEnableSensorEvents(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2Shape_EnableSensorEvents(id, flag);

    return 0;
}

int shapeAreSensorEventsEnabled(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushboolean(L, b2Shape_AreSensorEventsEnabled(id));

    return 1;
}

int shapeEnableContactEvents(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2Shape_EnableContactEvents(id, flag);

    return 0;
}

int shapeAreContactEventsEnabled(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushboolean(L, b2Shape_AreContactEventsEnabled(id));

    return 1;
}

int shapeEnableHitEvents(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2Shape_EnableHitEvents(id, flag);

    return 0;
}

int shapeAreHitEventsEnabled(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushboolean(L, b2Shape_AreHitEventsEnabled(id));

    return 1;
}

int shapeTestPoint(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    b2Pos point = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    lua_pushboolean(L, b2Shape_TestPoint(id, point));

    return 1;
}

int shapeGetCircle(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushCircle(L, b2Shape_GetCircle(id));

    return 1;
}

int shapeGetSegment(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushSegment(L, b2Shape_GetSegment(id));

    return 1;
}

int shapeGetCapsule(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushCapsule(L, b2Shape_GetCapsule(id));

    return 1;
}

int shapeGetPolygon(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushPolygon(L, b2Shape_GetPolygon(id));

    return 1;
}

int shapeSetCircle(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The circle must be a table");
        return 0;
    }

    b2Circle circle = constructCircle(L, 2);

    b2Shape_SetCircle(id, &circle);

    return 0;
}

int shapeSetCapsule(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The capsule must be a table");
        return 0;
    }

    b2Capsule capsule = constructCapsule(L, 2);

    b2Shape_SetCapsule(id, &capsule);

    return 0;
}

int shapeSetSegment(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The segment must be a table");
        return 0;
    }

    b2Segment segment = constructSegment(L, 2);

    b2Shape_SetSegment(id, &segment);

    return 0;
}

int shapeSetPolygon(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The polygon must be a table");
        return 0;
    }

    b2Polygon polygon = constructPolygon(L, 2);

    b2Shape_SetPolygon(id, &polygon);

    return 0;
}

int shapeGetContactCapacity(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushinteger(L, b2Shape_GetContactCapacity(id));

    return 1;
}

int shapeGetSensorCapacity(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushinteger(L, b2Shape_GetSensorCapacity(id));

    return 1;
}

int shapeGetAABB(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    b2AABB aabb = b2Shape_GetAABB(id);

    lua_pushnumber(L, aabb.lowerBound.x);
    lua_pushnumber(L, aabb.lowerBound.y);
    lua_pushnumber(L, aabb.upperBound.x);
    lua_pushnumber(L, aabb.upperBound.y);

    return 4;
}

int shapeComputeMassData(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    b2MassData massData = b2Shape_ComputeMassData(id);

    pushMassData(L, massData);

    return 1;
}

int shapeGetClosestPoint(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    b2Pos target = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Pos point = b2Shape_GetClosestPoint(id, target);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int shapeApplyWind(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    b2Vec2 wind = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    float drag = lua_tonumber(L, 4);
    float lift = lua_tonumber(L, 5);
    bool wake = lua_toboolean(L, 6);

    b2Shape_ApplyWind(id, wind, drag, lift, wake);

    return 0;
}

int createChainSegmentShape(lua_State* L) {
    b2BodyId bodyId = getBodyIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The shape definition must be a table");
        return 0;
    }

    if (!lua_istable(L, 3)) {
        luaL_argerror(L, 3, "The chain segment must be a table");
        return 0;
    }

    b2ShapeDef def = constructShapeDef(L, 2);
    b2ChainSegment chainSegment = constructChainSegment(L, 3);

    b2ShapeId shapeId = b2CreateChainSegmentShape(bodyId, &def, &chainSegment);

    pushShapeId(L, shapeId);

    return 1;
}

int shapeGetChainSegment(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushChainSegment(L, b2Shape_GetChainSegment(id));

    return 1;
}

int shapeSetChainSegment(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The chain segment must be a table");
        return 0;
    }

    b2ChainSegment chainSegment = b2Shape_GetChainSegment(id);
    applyChainSegment(L, 2, chainSegment);

    b2Shape_SetChainSegment(id, &chainSegment);

    return 0;
}

int shapeGetParentChain(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    pushChainId(L, b2Shape_GetParentChain(id));

    return 1;
}

// Chain

b2ChainId getChainIdFromLuau(lua_State* L) {
    if (lua_type(L, 1) != LUA_TINTEGER) {
        luaL_argerror(L, 1, "The chain ID must be an integer");
        return {};
    }

    Uint64 storedId = lua_tointeger64(L, 1, nullptr);
    return b2LoadChainId(storedId);
}

int createChain(lua_State* L) {
    b2BodyId bodyId = getBodyIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The chain definition must be a table");
        return 0;
    }

    b2ChainDef def = b2DefaultChainDef();

    lua_getfield(L, 2, "points");

    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        luaL_argerror(L, 2, "The chain definition must contain a points table");
        return 0;
    }

    int pointCount = lua_objlen(L, -1);

    if (pointCount < 4) {
        lua_pop(L, 1);
        luaL_argerror(L, 2, "A chain must contain at least 4 points");
        return 0;
    }

    b2Vec2* points = new b2Vec2[pointCount];

    for (int i = 0; i < pointCount; ++i) {
        lua_rawgeti(L, -1, i + 1);

        if (!lua_istable(L, -1)) {
            lua_pop(L, 2);
            delete[] points;
            luaL_argerror(L, 2, "Each chain point must be a table");
            return 0;
        }

        points[i] = {};
        applyVec2(L, lua_gettop(L), points[i]);

        lua_pop(L, 1);
    }

    lua_pop(L, 1);

    def.points = points;
    def.count = pointCount;

    b2SurfaceMaterial* materials = nullptr;

    lua_getfield(L, 2, "materials");

    if (lua_istable(L, -1)) {
        int materialCount = (int) (lua_objlen(L, -1));

        if (materialCount != 1 && materialCount != pointCount) {
            lua_pop(L, 1);
            delete[] points;
            luaL_argerror(L, 2, "Chain materials must contain either 1 entry or one entry per point");
            return 0;
        }

        materials = new b2SurfaceMaterial[materialCount];

        for (int i = 0; i < materialCount; ++i) {
            lua_rawgeti(L, -1, i + 1);

            if (!lua_istable(L, -1)) {
                lua_pop(L, 2);
                delete[] materials;
                delete[] points;
                luaL_argerror(L, 2, "Each chain material must be a table");
                return 0;
            }

            materials[i] = b2DefaultSurfaceMaterial();
            applySurfaceMaterial(L, lua_gettop(L), materials[i]);

            lua_pop(L, 1);
        }

        def.materials = materials;
        def.materialCount = materialCount;
    }

    lua_pop(L, 1);

    lua_getfield(L, 2, "filter");
    if (lua_istable(L, -1)) {
        applyFilter(L, lua_gettop(L), def.filter);
    }
    lua_pop(L, 1);

    lua_getfield(L, 2, "isLoop");
    if (!lua_isnil(L, -1)) {
        def.isLoop = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, 2, "enableSensorEvents");
    if (!lua_isnil(L, -1)) {
        def.enableSensorEvents = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    b2ChainId chainId = b2CreateChain(bodyId, &def);

    delete[] materials;
    delete[] points;

    pushChainId(L, chainId);

    return 1;
}

int destroyChain(lua_State* L) {
    b2ChainId id = getChainIdFromLuau(L);

    b2DestroyChain(id);

    return 0;
}

int chainIsValid(lua_State* L) {
    b2ChainId id = getChainIdFromLuau(L);

    lua_pushboolean(L, b2Chain_IsValid(id));

    return 1;
}

int chainGetWorld(lua_State* L) {
    b2ChainId id = getChainIdFromLuau(L);

    pushWorldId(L, b2Chain_GetWorld(id));

    return 1;
}

int chainGetSegmentCount(lua_State* L) {
    b2ChainId id = getChainIdFromLuau(L);

    lua_pushinteger(L, b2Chain_GetSegmentCount(id));

    return 1;
}

int chainGetSegments(lua_State* L) {
    b2ChainId id = getChainIdFromLuau(L);
    int capacity = b2Chain_GetSegmentCount(id);

    if (capacity <= 0) {
        lua_createtable(L, 0, 0);
        return 1;
    }

    b2ShapeId stackSegments[32];
    b2ShapeId* segments = (capacity <= 32) ? stackSegments : new b2ShapeId[capacity];
    int count = b2Chain_GetSegments(id, segments, capacity);

    lua_createtable(L, count, 0);

    for (int i = 0; i < count; ++i) {
        pushShapeId(L, segments[i]);
        lua_rawseti(L, -2, i + 1);
    }

    if (segments != stackSegments) delete[] segments;

    return 1;
}

int chainGetSurfaceMaterialCount(lua_State* L) {
    b2ChainId id = getChainIdFromLuau(L);

    lua_pushinteger(L, b2Chain_GetSurfaceMaterialCount(id));

    return 1;
}

int chainSetSurfaceMaterial(lua_State* L) {
    b2ChainId id = getChainIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The surface material must be a table");
        return 0;
    }

    int materialIndex = lua_tointeger(L, 3) - 1;

    b2SurfaceMaterial material = b2Chain_GetSurfaceMaterial(id, materialIndex);
    applySurfaceMaterial(L, 2, material);

    b2Chain_SetSurfaceMaterial(id, &material, materialIndex);

    return 0;
}

int chainGetSurfaceMaterial(lua_State* L) {
    b2ChainId id = getChainIdFromLuau(L);
    int materialIndex = lua_tointeger(L, 2) - 1;

    pushSurfaceMaterial(L, b2Chain_GetSurfaceMaterial(id, materialIndex));

    return 1;
}

// Make

b2Hull constructHull(lua_State* L, int index) {
    b2Hull hull = {};

    lua_getfield(L, index, "count");
    if (!lua_isnil(L, -1)) {
        hull.count = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    if (hull.count < 0) {
        hull.count = 0;
    }
    else if (hull.count > B2_MAX_POLYGON_VERTICES) {
        hull.count = B2_MAX_POLYGON_VERTICES;
    }

    lua_getfield(L, index, "points");
    if (lua_istable(L, -1)) {
        int pointsIndex = lua_gettop(L);

        for (int i = 0; i < hull.count; ++i) {
            lua_rawgeti(L, pointsIndex, i + 1);

            if (lua_istable(L, -1)) {
                applyVec2(L, lua_gettop(L), hull.points[i]);
            }

            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    return hull;
}

void pushHull(lua_State* L, b2Hull hull) {
    lua_createtable(L, 0, 2);

    lua_createtable(L, hull.count, 0);

    for (int i = 0; i < hull.count; ++i) {
        pushVec2(L, hull.points[i]);
        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "points");

    lua_pushinteger(L, hull.count);
    lua_setfield(L, -2, "count");
}

int makeOffsetRoundedBox(lua_State* L) {
    float halfWidth = lua_tonumber(L, 1);
    float halfHeight = lua_tonumber(L, 2);

    b2Vec2 center = {
        (float) (lua_tonumber(L, 3)),
        (float) (lua_tonumber(L, 4))
    };

    b2Rot rotation = {
        (float) (lua_tonumber(L, 5)),
        (float) (lua_tonumber(L, 6))
    };

    float radius = lua_tonumber(L, 7);

    b2Polygon polygon = b2MakeOffsetRoundedBox(halfWidth, halfHeight, center, rotation, radius);
    pushPolygon(L, polygon);

    return 1;
}

int makeOffsetBox(lua_State* L) {
    float halfWidth = lua_tonumber(L, 1);
    float halfHeight = lua_tonumber(L, 2);

    b2Vec2 center = {
        (float) (lua_tonumber(L, 3)),
        (float) (lua_tonumber(L, 4))
    };

    b2Rot rotation = {
        (float) (lua_tonumber(L, 5)),
        (float) (lua_tonumber(L, 6))
    };

    b2Polygon polygon = b2MakeOffsetBox(halfWidth, halfHeight, center, rotation);
    pushPolygon(L, polygon);

    return 1;
}

int makeRoundedBox(lua_State* L) {
    float halfWidth = lua_tonumber(L, 1);
    float halfHeight = lua_tonumber(L, 2);
    float radius = lua_tonumber(L, 3);

    b2Polygon polygon = b2MakeRoundedBox(halfWidth, halfHeight, radius);
    pushPolygon(L, polygon);

    return 1;
}

int makeBox(lua_State* L) {
    float halfWidth = lua_tonumber(L, 1);
    float halfHeight = lua_tonumber(L, 2);

    b2Polygon polygon = b2MakeBox(halfWidth, halfHeight);
    pushPolygon(L, polygon);

    return 1;
}

int makeSquare(lua_State* L) {
    float halfWidth = lua_tonumber(L, 1);

    b2Polygon polygon = b2MakeSquare(halfWidth);
    pushPolygon(L, polygon);

    return 1;
}

int makeOffsetRoundedPolygon(lua_State* L) {
    if (!lua_istable(L, 1)) {
        luaL_argerror(L, 1, "The hull must be a table");
        return 0;
    }

    b2Hull hull = constructHull(L, 1);

    b2Vec2 position = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Rot rotation = {
        (float) (lua_tonumber(L, 4)),
        (float) (lua_tonumber(L, 5))
    };

    float radius = lua_tonumber(L, 6);

    b2Polygon polygon = b2MakeOffsetRoundedPolygon(&hull, position, rotation, radius);
    pushPolygon(L, polygon);

    return 1;
}

int makeOffsetPolygon(lua_State* L) {
    if (!lua_istable(L, 1)) {
        luaL_argerror(L, 1, "The hull must be a table");
        return 0;
    }

    b2Hull hull = constructHull(L, 1);

    b2Vec2 position = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2Rot rotation = {
        (float) (lua_tonumber(L, 4)),
        (float) (lua_tonumber(L, 5))
    };

    b2Polygon polygon = b2MakeOffsetPolygon(&hull, position, rotation);
    pushPolygon(L, polygon);

    return 1;
}

int makePolygon(lua_State* L) {
    if (!lua_istable(L, 1)) {
        luaL_argerror(L, 1, "The hull must be a table");
        return 0;
    }

    b2Hull hull = constructHull(L, 1);
    float radius = lua_tonumber(L, 2);

    b2Polygon polygon = b2MakePolygon(&hull, radius);
    pushPolygon(L, polygon);

    return 1;
}

int computeHull(lua_State* L) {
    if (!lua_istable(L, 1)) {
        luaL_argerror(L, 1, "The points must be a table");
        return 0;
    }

    int count = (int) (lua_objlen(L, 1));

    if (count <= 0) {
        luaL_argerror(L, 1, "The points table must not be empty");
        return 0;
    }

    if (count > B2_MAX_POLYGON_VERTICES) {
        luaL_argerror(L, 1, "Too many hull points");
        return 0;
    }

    b2Vec2 points[B2_MAX_POLYGON_VERTICES] = {};

    for (int i = 0; i < count; ++i) {
        lua_rawgeti(L, 1, i + 1);

        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            luaL_argerror(L, 1, "Each hull point must be a table");
            return 0;
        }

        applyVec2(L, lua_gettop(L), points[i]);
        lua_pop(L, 1);
    }

    b2Hull hull = b2ComputeHull(points, count);

    pushHull(L, hull);

    return 1;
}

// Joints

b2JointId getJointIdFromLuau(lua_State* L) {
    if (lua_type(L, 1) != LUA_TINTEGER) {
        luaL_argerror(L, 1, "The joint ID must be an integer");
        return {};
    }

    Uint64 storedId = lua_tointeger64(L, 1, nullptr);
    return b2LoadJointId(storedId);
}

void pushJointId(lua_State* L, b2JointId id) {
    lua_pushinteger64(L, b2StoreJointId(id));
}

b2Transform constructTransform(lua_State* L, int index) {
    b2Transform transform = {};

    lua_getfield(L, index, "p");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), transform.p);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "q");
    if (lua_istable(L, -1)) {
        int rotationIndex = lua_gettop(L);

        lua_getfield(L, rotationIndex, "c");
        if (!lua_isnil(L, -1)) {
            transform.q.c = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, rotationIndex, "s");
        if (!lua_isnil(L, -1)) {
            transform.q.s = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return transform;
}

void pushTransform(lua_State* L, b2Transform transform) {
    lua_createtable(L, 0, 2);

    pushVec2(L, transform.p);
    lua_setfield(L, -2, "p");

    lua_createtable(L, 0, 2);

    lua_pushnumber(L, transform.q.c);
    lua_setfield(L, -2, "c");

    lua_pushnumber(L, transform.q.s);
    lua_setfield(L, -2, "s");

    lua_setfield(L, -2, "q");
}

int worldGetJointEvents(lua_State* L) {
    b2WorldId id = getWorldIdFromLuau(L);
    b2JointEvents events = b2World_GetJointEvents(id);

    lua_createtable(L, 0, 2);

    lua_createtable(L, events.count, 0);

    for (int i = 0; i < events.count; ++i) {
        const b2JointEvent& event = events.jointEvents[i];

        lua_createtable(L, 0, 1);

        pushJointId(L, event.jointId);
        lua_setfield(L, -2, "jointId");

        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "jointEvents");

    lua_pushinteger(L, events.count);
    lua_setfield(L, -2, "count");

    return 1;
}

int destroyJoint(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    b2DestroyJoint(id);

    return 0;
}

int jointIsValid(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2Joint_IsValid(id));

    return 1;
}

int jointGetType(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushinteger(L, b2Joint_GetType(id));

    return 1;
}

int jointGetBodyA(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    pushBodyId(L, b2Joint_GetBodyA(id));

    return 1;
}

int jointGetBodyB(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    pushBodyId(L, b2Joint_GetBodyB(id));

    return 1;
}

int jointGetWorld(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    pushWorldId(L, b2Joint_GetWorld(id));

    return 1;
}

int jointSetLocalFrameA(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The local frame must be a table");
        return 0;
    }

    b2Transform frame = constructTransform(L, 2);
    b2Joint_SetLocalFrameA(id, frame);

    return 0;
}

int jointGetLocalFrameA(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    pushTransform(L, b2Joint_GetLocalFrameA(id));

    return 1;
}

int jointSetLocalFrameB(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The local frame must be a table");
        return 0;
    }

    b2Transform frame = constructTransform(L, 2);
    b2Joint_SetLocalFrameB(id, frame);

    return 0;
}

int jointGetLocalFrameB(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    pushTransform(L, b2Joint_GetLocalFrameB(id));

    return 1;
}

int jointSetCollideConnected(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool shouldCollide = lua_toboolean(L, 2);

    b2Joint_SetCollideConnected(id, shouldCollide);

    return 0;
}

int jointGetCollideConnected(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2Joint_GetCollideConnected(id));

    return 1;
}

int jointWakeBodies(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    b2Joint_WakeBodies(id);

    return 0;
}

int jointGetConstraintForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    b2Vec2 force = b2Joint_GetConstraintForce(id);

    lua_pushnumber(L, force.x);
    lua_pushnumber(L, force.y);

    return 2;
}

int jointGetConstraintTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2Joint_GetConstraintTorque(id));

    return 1;
}

int jointGetLinearSeparation(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2Joint_GetLinearSeparation(id));

    return 1;
}

int jointGetAngularSeparation(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2Joint_GetAngularSeparation(id));

    return 1;
}

int jointSetConstraintTuning(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);
    float dampingRatio = lua_tonumber(L, 3);

    b2Joint_SetConstraintTuning(id, hertz, dampingRatio);

    return 0;
}

int jointGetConstraintTuning(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    float hertz = 0.0f;
    float dampingRatio = 0.0f;

    b2Joint_GetConstraintTuning(id, &hertz, &dampingRatio);

    lua_pushnumber(L, hertz);
    lua_pushnumber(L, dampingRatio);

    return 2;
}

int jointSetForceThreshold(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float threshold = lua_tonumber(L, 2);

    b2Joint_SetForceThreshold(id, threshold);

    return 0;
}

int jointGetForceThreshold(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2Joint_GetForceThreshold(id));

    return 1;
}

int jointSetTorqueThreshold(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float threshold = lua_tonumber(L, 2);

    b2Joint_SetTorqueThreshold(id, threshold);

    return 0;
}

int jointGetTorqueThreshold(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2Joint_GetTorqueThreshold(id));

    return 1;
}

// Distance Joint

b2BodyId constructBodyId(lua_State* L, int index) {
    if (lua_type(L, index) != LUA_TINTEGER) {
        luaL_argerror(L, index, "The body ID must be an integer");
        return {};
    }

    Uint64 storedId = lua_tointeger64(L, index, nullptr);
    return b2LoadBodyId(storedId);
}

void applyTransform(lua_State* L, int index, b2Transform& transform) {
    if (!lua_istable(L, index)) {
        return;
    }

    lua_getfield(L, index, "p");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), transform.p);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "q");
    if (lua_istable(L, -1)) {
        int rotationIndex = lua_gettop(L);

        lua_getfield(L, rotationIndex, "c");
        if (!lua_isnil(L, -1)) {
            transform.q.c = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, rotationIndex, "s");
        if (!lua_isnil(L, -1)) {
            transform.q.s = lua_tonumber(L, -1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

void applyJointDef(lua_State* L, int index, b2JointDef& def) {
    lua_getfield(L, index, "bodyIdA");
    if (!lua_isnil(L, -1)) {
        def.bodyIdA = constructBodyId(L, lua_gettop(L));
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "bodyIdB");
    if (!lua_isnil(L, -1)) {
        def.bodyIdB = constructBodyId(L, lua_gettop(L));
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "localFrameA");
    if (lua_istable(L, -1)) {
        applyTransform(L, lua_gettop(L), def.localFrameA);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "localFrameB");
    if (lua_istable(L, -1)) {
        applyTransform(L, lua_gettop(L), def.localFrameB);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "forceThreshold");
    if (!lua_isnil(L, -1)) {
        def.forceThreshold = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "torqueThreshold");
    if (!lua_isnil(L, -1)) {
        def.torqueThreshold = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "constraintHertz");
    if (!lua_isnil(L, -1)) {
        def.constraintHertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "constraintDampingRatio");
    if (!lua_isnil(L, -1)) {
        def.constraintDampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "drawScale");
    if (!lua_isnil(L, -1)) {
        def.drawScale = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "collideConnected");
    if (!lua_isnil(L, -1)) {
        def.collideConnected = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
}

b2DistanceJointDef constructDistanceJointDef(lua_State* L, int index) {
    b2DistanceJointDef def = b2DefaultDistanceJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "length");
    if (!lua_isnil(L, -1)) {
        def.length = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableSpring");
    if (!lua_isnil(L, -1)) {
        def.enableSpring = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "lowerSpringForce");
    if (!lua_isnil(L, -1)) {
        def.lowerSpringForce = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "upperSpringForce");
    if (!lua_isnil(L, -1)) {
        def.upperSpringForce = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "hertz");
    if (!lua_isnil(L, -1)) {
        def.hertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "dampingRatio");
    if (!lua_isnil(L, -1)) {
        def.dampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableLimit");
    if (!lua_isnil(L, -1)) {
        def.enableLimit = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "minLength");
    if (!lua_isnil(L, -1)) {
        def.minLength = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxLength");
    if (!lua_isnil(L, -1)) {
        def.maxLength = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableMotor");
    if (!lua_isnil(L, -1)) {
        def.enableMotor = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxMotorForce");
    if (!lua_isnil(L, -1)) {
        def.maxMotorForce = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "motorSpeed");
    if (!lua_isnil(L, -1)) {
        def.motorSpeed = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return def;
}

int createDistanceJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The distance joint definition must be a table");
        return 0;
    }

    b2DistanceJointDef def = constructDistanceJointDef(L, 2);
    b2JointId jointId = b2CreateDistanceJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

int distanceJointSetLength(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float length = lua_tonumber(L, 2);

    b2DistanceJoint_SetLength(id, length);

    return 0;
}

int distanceJointGetLength(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetLength(id));

    return 1;
}

int distanceJointGetCurrentLength(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetCurrentLength(id));

    return 1;
}

int distanceJointEnableSpring(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableSpring = lua_toboolean(L, 2);

    b2DistanceJoint_EnableSpring(id, enableSpring);

    return 0;
}

int distanceJointIsSpringEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2DistanceJoint_IsSpringEnabled(id));

    return 1;
}

int distanceJointSetSpringForceRange(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float lowerForce = lua_tonumber(L, 2);
    float upperForce = lua_tonumber(L, 3);

    b2DistanceJoint_SetSpringForceRange(id, lowerForce, upperForce);

    return 0;
}

int distanceJointGetSpringForceRange(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    float lowerForce = 0.0f;
    float upperForce = 0.0f;

    b2DistanceJoint_GetSpringForceRange(id, &lowerForce, &upperForce);

    lua_pushnumber(L, lowerForce);
    lua_pushnumber(L, upperForce);

    return 2;
}

int distanceJointSetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2DistanceJoint_SetSpringHertz(id, hertz);

    return 0;
}

int distanceJointGetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetSpringHertz(id));

    return 1;
}

int distanceJointSetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float dampingRatio = lua_tonumber(L, 2);

    b2DistanceJoint_SetSpringDampingRatio(id, dampingRatio);

    return 0;
}

int distanceJointGetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetSpringDampingRatio(id));

    return 1;
}

int distanceJointEnableLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableLimit = lua_toboolean(L, 2);

    b2DistanceJoint_EnableLimit(id, enableLimit);

    return 0;
}

int distanceJointIsLimitEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2DistanceJoint_IsLimitEnabled(id));

    return 1;
}

int distanceJointSetLengthRange(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float minLength = lua_tonumber(L, 2);
    float maxLength = lua_tonumber(L, 3);

    b2DistanceJoint_SetLengthRange(id, minLength, maxLength);

    return 0;
}

int distanceJointGetMinLength(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetMinLength(id));

    return 1;
}

int distanceJointGetMaxLength(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetMaxLength(id));

    return 1;
}

int distanceJointEnableMotor(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableMotor = lua_toboolean(L, 2);

    b2DistanceJoint_EnableMotor(id, enableMotor);

    return 0;
}

int distanceJointIsMotorEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2DistanceJoint_IsMotorEnabled(id));

    return 1;
}

int distanceJointSetMotorSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float motorSpeed = lua_tonumber(L, 2);

    b2DistanceJoint_SetMotorSpeed(id, motorSpeed);

    return 0;
}

int distanceJointGetMotorSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetMotorSpeed(id));

    return 1;
}

int distanceJointSetMaxMotorForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float force = lua_tonumber(L, 2);

    b2DistanceJoint_SetMaxMotorForce(id, force);

    return 0;
}

int distanceJointGetMaxMotorForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetMaxMotorForce(id));

    return 1;
}

int distanceJointGetMotorForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2DistanceJoint_GetMotorForce(id));

    return 1;
}

// Filter joint

b2FilterJointDef constructFilterJointDef(lua_State* L, int index) {
    b2FilterJointDef def = b2DefaultFilterJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    return def;
}

int createFilterJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The filter joint definition must be a table");
        return 0;
    }

    b2FilterJointDef def = constructFilterJointDef(L, 2);
    b2JointId jointId = b2CreateFilterJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

// Motor joint

b2MotorJointDef constructMotorJointDef(lua_State* L, int index) {
    b2MotorJointDef def = b2DefaultMotorJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "linearVelocity");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), def.linearVelocity);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxVelocityForce");
    if (!lua_isnil(L, -1)) {
        def.maxVelocityForce = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "angularVelocity");
    if (!lua_isnil(L, -1)) {
        def.angularVelocity = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxVelocityTorque");
    if (!lua_isnil(L, -1)) {
        def.maxVelocityTorque = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "linearHertz");
    if (!lua_isnil(L, -1)) {
        def.linearHertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "linearDampingRatio");
    if (!lua_isnil(L, -1)) {
        def.linearDampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxSpringForce");
    if (!lua_isnil(L, -1)) {
        def.maxSpringForce = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "angularHertz");
    if (!lua_isnil(L, -1)) {
        def.angularHertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "angularDampingRatio");
    if (!lua_isnil(L, -1)) {
        def.angularDampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxSpringTorque");
    if (!lua_isnil(L, -1)) {
        def.maxSpringTorque = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return def;
}

int createMotorJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The motor joint definition must be a table");
        return 0;
    }

    b2MotorJointDef def = constructMotorJointDef(L, 2);
    b2JointId jointId = b2CreateMotorJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

int motorJointSetLinearVelocity(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    b2Vec2 velocity = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2MotorJoint_SetLinearVelocity(id, velocity);

    return 0;
}

int motorJointGetLinearVelocity(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    b2Vec2 velocity = b2MotorJoint_GetLinearVelocity(id);

    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);

    return 2;
}

int motorJointSetAngularVelocity(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float velocity = lua_tonumber(L, 2);

    b2MotorJoint_SetAngularVelocity(id, velocity);

    return 0;
}

int motorJointGetAngularVelocity(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetAngularVelocity(id));

    return 1;
}

int motorJointSetMaxVelocityForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float maxForce = lua_tonumber(L, 2);

    b2MotorJoint_SetMaxVelocityForce(id, maxForce);

    return 0;
}

int motorJointGetMaxVelocityForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetMaxVelocityForce(id));

    return 1;
}

int motorJointSetMaxVelocityTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float maxTorque = lua_tonumber(L, 2);

    b2MotorJoint_SetMaxVelocityTorque(id, maxTorque);

    return 0;
}

int motorJointGetMaxVelocityTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetMaxVelocityTorque(id));

    return 1;
}

int motorJointSetLinearHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2MotorJoint_SetLinearHertz(id, hertz);

    return 0;
}

int motorJointGetLinearHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetLinearHertz(id));

    return 1;
}

int motorJointSetLinearDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float damping = lua_tonumber(L, 2);

    b2MotorJoint_SetLinearDampingRatio(id, damping);

    return 0;
}

int motorJointGetLinearDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetLinearDampingRatio(id));

    return 1;
}

int motorJointSetAngularHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2MotorJoint_SetAngularHertz(id, hertz);

    return 0;
}

int motorJointGetAngularHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetAngularHertz(id));

    return 1;
}

int motorJointSetAngularDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float damping = lua_tonumber(L, 2);

    b2MotorJoint_SetAngularDampingRatio(id, damping);

    return 0;
}

int motorJointGetAngularDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetAngularDampingRatio(id));

    return 1;
}

int motorJointSetMaxSpringForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float maxForce = lua_tonumber(L, 2);

    b2MotorJoint_SetMaxSpringForce(id, maxForce);

    return 0;
}

int motorJointGetMaxSpringForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetMaxSpringForce(id));

    return 1;
}

int motorJointSetMaxSpringTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float maxTorque = lua_tonumber(L, 2);

    b2MotorJoint_SetMaxSpringTorque(id, maxTorque);

    return 0;
}

int motorJointGetMaxSpringTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2MotorJoint_GetMaxSpringTorque(id));

    return 1;
}

// Mover joint

b2MoverJointDef constructMoverJointDef(lua_State* L, int index) {
    b2MoverJointDef def = b2DefaultMoverJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "linearVelocity");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), def.linearVelocity);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxVelocityForce");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), def.maxVelocityForce);
    }
    lua_pop(L, 1);

    return def;
}

int createMoverJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The mover joint definition must be a table");
        return 0;
    }

    b2MoverJointDef def = constructMoverJointDef(L, 2);
    b2JointId jointId = b2CreateMoverJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

int moverJointSetLinearVelocity(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    b2Vec2 velocity = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2MoverJoint_SetLinearVelocity(id, velocity);

    return 0;
}

int moverJointGetLinearVelocity(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    b2Vec2 velocity = b2MoverJoint_GetLinearVelocity(id);

    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);

    return 2;
}

int moverJointSetMaxVelocityForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    b2Vec2 maxForce = {
        (float) (lua_tonumber(L, 2)),
        (float) (lua_tonumber(L, 3))
    };

    b2MoverJoint_SetMaxVelocityForce(id, maxForce);

    return 0;
}

int moverJointGetMaxVelocityForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    b2Vec2 maxForce = b2MoverJoint_GetMaxVelocityForce(id);

    lua_pushnumber(L, maxForce.x);
    lua_pushnumber(L, maxForce.y);

    return 2;
}

// Pogo joint

b2PogoJointDef constructPogoJointDef(lua_State* L, int index) {
    b2PogoJointDef def = b2DefaultPogoJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "normal");
    if (lua_istable(L, -1)) {
        applyVec2(L, lua_gettop(L), def.normal);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "hertz");
    if (!lua_isnil(L, -1)) {
        def.hertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "dampingRatio");
    if (!lua_isnil(L, -1)) {
        def.dampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "restLength");
    if (!lua_isnil(L, -1)) {
        def.restLength = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxTensionForce");
    if (!lua_isnil(L, -1)) {
        def.maxTensionForce = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxCompressionForce");
    if (!lua_isnil(L, -1)) {
        def.maxCompressionForce = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "impulse");
    if (!lua_isnil(L, -1)) {
        def.impulse = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "velocity");
    if (!lua_isnil(L, -1)) {
        def.velocity = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    // internalValue intentionally untouched.

    return def;
}

int createPogoJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The pogo joint definition must be a table");
        return 0;
    }

    b2PogoJointDef def = constructPogoJointDef(L, 2);
    b2JointId jointId = b2CreatePogoJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

int pogoJointSetRestLength(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float length = lua_tonumber(L, 2);

    b2PogoJoint_SetRestLength(id, length);

    return 0;
}

int pogoJointGetRestLength(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PogoJoint_GetRestLength(id));

    return 1;
}

int pogoJointSetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2PogoJoint_SetSpringHertz(id, hertz);

    return 0;
}

int pogoJointGetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PogoJoint_GetSpringHertz(id));

    return 1;
}

int pogoJointSetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float dampingRatio = lua_tonumber(L, 2);

    b2PogoJoint_SetSpringDampingRatio(id, dampingRatio);

    return 0;
}

int pogoJointGetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PogoJoint_GetSpringDampingRatio(id));

    return 1;
}

int pogoJointGetLength(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PogoJoint_GetLength(id));

    return 1;
}

int pogoJointGetVelocity(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PogoJoint_GetVelocity(id));

    return 1;
}

int pogoJointGetImpulse(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PogoJoint_GetImpulse(id));

    return 1;
}

// Prismatic joint

b2PrismaticJointDef constructPrismaticJointDef(lua_State* L, int index) {
    b2PrismaticJointDef def = b2DefaultPrismaticJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableSpring");
    if (!lua_isnil(L, -1)) {
        def.enableSpring = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "hertz");
    if (!lua_isnil(L, -1)) {
        def.hertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "dampingRatio");
    if (!lua_isnil(L, -1)) {
        def.dampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "targetTranslation");
    if (!lua_isnil(L, -1)) {
        def.targetTranslation = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableLimit");
    if (!lua_isnil(L, -1)) {
        def.enableLimit = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "lowerTranslation");
    if (!lua_isnil(L, -1)) {
        def.lowerTranslation = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "upperTranslation");
    if (!lua_isnil(L, -1)) {
        def.upperTranslation = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableMotor");
    if (!lua_isnil(L, -1)) {
        def.enableMotor = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxMotorForce");
    if (!lua_isnil(L, -1)) {
        def.maxMotorForce = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "motorSpeed");
    if (!lua_isnil(L, -1)) {
        def.motorSpeed = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    // internalValue intentionally untouched.

    return def;
}

int createPrismaticJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The prismatic joint definition must be a table");
        return 0;
    }

    b2PrismaticJointDef def = constructPrismaticJointDef(L, 2);
    b2JointId jointId = b2CreatePrismaticJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

int prismaticJointEnableSpring(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableSpring = lua_toboolean(L, 2);

    b2PrismaticJoint_EnableSpring(id, enableSpring);

    return 0;
}

int prismaticJointIsSpringEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2PrismaticJoint_IsSpringEnabled(id));

    return 1;
}

int prismaticJointSetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2PrismaticJoint_SetSpringHertz(id, hertz);

    return 0;
}

int prismaticJointGetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetSpringHertz(id));

    return 1;
}

int prismaticJointSetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float dampingRatio = lua_tonumber(L, 2);

    b2PrismaticJoint_SetSpringDampingRatio(id, dampingRatio);

    return 0;
}

int prismaticJointGetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetSpringDampingRatio(id));

    return 1;
}

int prismaticJointSetTargetTranslation(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float translation = lua_tonumber(L, 2);

    b2PrismaticJoint_SetTargetTranslation(id, translation);

    return 0;
}

int prismaticJointGetTargetTranslation(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetTargetTranslation(id));

    return 1;
}

int prismaticJointEnableLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableLimit = lua_toboolean(L, 2);

    b2PrismaticJoint_EnableLimit(id, enableLimit);

    return 0;
}

int prismaticJointIsLimitEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2PrismaticJoint_IsLimitEnabled(id));

    return 1;
}

int prismaticJointGetLowerLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetLowerLimit(id));

    return 1;
}

int prismaticJointGetUpperLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetUpperLimit(id));

    return 1;
}

int prismaticJointSetLimits(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float lower = lua_tonumber(L, 2);
    float upper = lua_tonumber(L, 3);

    b2PrismaticJoint_SetLimits(id, lower, upper);

    return 0;
}

int prismaticJointEnableMotor(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableMotor = lua_toboolean(L, 2);

    b2PrismaticJoint_EnableMotor(id, enableMotor);

    return 0;
}

int prismaticJointIsMotorEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2PrismaticJoint_IsMotorEnabled(id));

    return 1;
}

int prismaticJointSetMotorSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float motorSpeed = lua_tonumber(L, 2);

    b2PrismaticJoint_SetMotorSpeed(id, motorSpeed);

    return 0;
}

int prismaticJointGetMotorSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetMotorSpeed(id));

    return 1;
}

int prismaticJointSetMaxMotorForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float force = lua_tonumber(L, 2);

    b2PrismaticJoint_SetMaxMotorForce(id, force);

    return 0;
}

int prismaticJointGetMaxMotorForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetMaxMotorForce(id));

    return 1;
}

int prismaticJointGetMotorForce(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetMotorForce(id));

    return 1;
}

int prismaticJointGetTranslation(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetTranslation(id));

    return 1;
}

int prismaticJointGetSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2PrismaticJoint_GetSpeed(id));

    return 1;
}

// Revolute joint

b2RevoluteJointDef constructRevoluteJointDef(lua_State* L, int index) {
    b2RevoluteJointDef def = b2DefaultRevoluteJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "targetAngle");
    if (!lua_isnil(L, -1)) {
        def.targetAngle = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableSpring");
    if (!lua_isnil(L, -1)) {
        def.enableSpring = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "hertz");
    if (!lua_isnil(L, -1)) {
        def.hertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "dampingRatio");
    if (!lua_isnil(L, -1)) {
        def.dampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableLimit");
    if (!lua_isnil(L, -1)) {
        def.enableLimit = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "lowerAngle");
    if (!lua_isnil(L, -1)) {
        def.lowerAngle = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "upperAngle");
    if (!lua_isnil(L, -1)) {
        def.upperAngle = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableMotor");
    if (!lua_isnil(L, -1)) {
        def.enableMotor = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxMotorTorque");
    if (!lua_isnil(L, -1)) {
        def.maxMotorTorque = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "motorSpeed");
    if (!lua_isnil(L, -1)) {
        def.motorSpeed = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    // internalValue intentionally untouched.

    return def;
}

int createRevoluteJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The revolute joint definition must be a table");
        return 0;
    }

    b2RevoluteJointDef def = constructRevoluteJointDef(L, 2);
    b2JointId jointId = b2CreateRevoluteJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

int revoluteJointEnableSpring(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableSpring = lua_toboolean(L, 2);

    b2RevoluteJoint_EnableSpring(id, enableSpring);

    return 0;
}

int revoluteJointIsSpringEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2RevoluteJoint_IsSpringEnabled(id));

    return 1;
}

int revoluteJointSetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2RevoluteJoint_SetSpringHertz(id, hertz);

    return 0;
}

int revoluteJointGetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetSpringHertz(id));

    return 1;
}

int revoluteJointSetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float dampingRatio = lua_tonumber(L, 2);

    b2RevoluteJoint_SetSpringDampingRatio(id, dampingRatio);

    return 0;
}

int revoluteJointGetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetSpringDampingRatio(id));

    return 1;
}

int revoluteJointSetTargetAngle(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float angle = lua_tonumber(L, 2);

    b2RevoluteJoint_SetTargetAngle(id, angle);

    return 0;
}

int revoluteJointGetTargetAngle(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetTargetAngle(id));

    return 1;
}

int revoluteJointGetAngle(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetAngle(id));

    return 1;
}

int revoluteJointEnableLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableLimit = lua_toboolean(L, 2);

    b2RevoluteJoint_EnableLimit(id, enableLimit);

    return 0;
}

int revoluteJointIsLimitEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2RevoluteJoint_IsLimitEnabled(id));

    return 1;
}

int revoluteJointGetLowerLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetLowerLimit(id));

    return 1;
}

int revoluteJointGetUpperLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetUpperLimit(id));

    return 1;
}

int revoluteJointSetLimits(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float lower = lua_tonumber(L, 2);
    float upper = lua_tonumber(L, 3);

    b2RevoluteJoint_SetLimits(id, lower, upper);

    return 0;
}

int revoluteJointEnableMotor(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableMotor = lua_toboolean(L, 2);

    b2RevoluteJoint_EnableMotor(id, enableMotor);

    return 0;
}

int revoluteJointIsMotorEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2RevoluteJoint_IsMotorEnabled(id));

    return 1;
}

int revoluteJointSetMotorSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float motorSpeed = lua_tonumber(L, 2);

    b2RevoluteJoint_SetMotorSpeed(id, motorSpeed);

    return 0;
}

int revoluteJointGetMotorSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetMotorSpeed(id));

    return 1;
}

int revoluteJointGetMotorTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetMotorTorque(id));

    return 1;
}

int revoluteJointSetMaxMotorTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float torque = lua_tonumber(L, 2);

    b2RevoluteJoint_SetMaxMotorTorque(id, torque);

    return 0;
}

int revoluteJointGetMaxMotorTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2RevoluteJoint_GetMaxMotorTorque(id));

    return 1;
}

// Weld joint

b2WeldJointDef constructWeldJointDef(lua_State* L, int index) {
    b2WeldJointDef def = b2DefaultWeldJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "linearHertz");
    if (!lua_isnil(L, -1)) {
        def.linearHertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "angularHertz");
    if (!lua_isnil(L, -1)) {
        def.angularHertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "linearDampingRatio");
    if (!lua_isnil(L, -1)) {
        def.linearDampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "angularDampingRatio");
    if (!lua_isnil(L, -1)) {
        def.angularDampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return def;
}

int createWeldJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The weld joint definition must be a table");
        return 0;
    }

    b2WeldJointDef def = constructWeldJointDef(L, 2);
    b2JointId jointId = b2CreateWeldJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

int weldJointSetLinearHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2WeldJoint_SetLinearHertz(id, hertz);

    return 0;
}

int weldJointGetLinearHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WeldJoint_GetLinearHertz(id));

    return 1;
}

int weldJointSetLinearDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float dampingRatio = lua_tonumber(L, 2);

    b2WeldJoint_SetLinearDampingRatio(id, dampingRatio);

    return 0;
}

int weldJointGetLinearDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WeldJoint_GetLinearDampingRatio(id));

    return 1;
}

int weldJointSetAngularHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2WeldJoint_SetAngularHertz(id, hertz);

    return 0;
}

int weldJointGetAngularHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WeldJoint_GetAngularHertz(id));

    return 1;
}

int weldJointSetAngularDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float dampingRatio = lua_tonumber(L, 2);

    b2WeldJoint_SetAngularDampingRatio(id, dampingRatio);

    return 0;
}

int weldJointGetAngularDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WeldJoint_GetAngularDampingRatio(id));

    return 1;
}

// Wheel joint

b2WheelJointDef constructWheelJointDef(lua_State* L, int index) {
    b2WheelJointDef def = b2DefaultWheelJointDef();

    lua_getfield(L, index, "base");
    if (lua_istable(L, -1)) {
        applyJointDef(L, lua_gettop(L), def.base);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableSpring");
    if (!lua_isnil(L, -1)) {
        def.enableSpring = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "hertz");
    if (!lua_isnil(L, -1)) {
        def.hertz = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "dampingRatio");
    if (!lua_isnil(L, -1)) {
        def.dampingRatio = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableLimit");
    if (!lua_isnil(L, -1)) {
        def.enableLimit = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "lowerTranslation");
    if (!lua_isnil(L, -1)) {
        def.lowerTranslation = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "upperTranslation");
    if (!lua_isnil(L, -1)) {
        def.upperTranslation = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "enableMotor");
    if (!lua_isnil(L, -1)) {
        def.enableMotor = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "maxMotorTorque");
    if (!lua_isnil(L, -1)) {
        def.maxMotorTorque = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, index, "motorSpeed");
    if (!lua_isnil(L, -1)) {
        def.motorSpeed = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    return def;
}

int createWheelJoint(lua_State* L) {
    b2WorldId worldId = getWorldIdFromLuau(L);

    if (!lua_istable(L, 2)) {
        luaL_argerror(L, 2, "The wheel joint definition must be a table");
        return 0;
    }

    b2WheelJointDef def = constructWheelJointDef(L, 2);
    b2JointId jointId = b2CreateWheelJoint(worldId, &def);

    pushJointId(L, jointId);

    return 1;
}

int wheelJointEnableSpring(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableSpring = lua_toboolean(L, 2);

    b2WheelJoint_EnableSpring(id, enableSpring);

    return 0;
}

int wheelJointIsSpringEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2WheelJoint_IsSpringEnabled(id));

    return 1;
}

int wheelJointSetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float hertz = lua_tonumber(L, 2);

    b2WheelJoint_SetSpringHertz(id, hertz);

    return 0;
}

int wheelJointGetSpringHertz(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WheelJoint_GetSpringHertz(id));

    return 1;
}

int wheelJointSetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float dampingRatio = lua_tonumber(L, 2);

    b2WheelJoint_SetSpringDampingRatio(id, dampingRatio);

    return 0;
}

int wheelJointGetSpringDampingRatio(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WheelJoint_GetSpringDampingRatio(id));

    return 1;
}

int wheelJointEnableLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableLimit = lua_toboolean(L, 2);

    b2WheelJoint_EnableLimit(id, enableLimit);

    return 0;
}

int wheelJointIsLimitEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2WheelJoint_IsLimitEnabled(id));

    return 1;
}

int wheelJointGetLowerLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WheelJoint_GetLowerLimit(id));

    return 1;
}

int wheelJointGetUpperLimit(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WheelJoint_GetUpperLimit(id));

    return 1;
}

int wheelJointSetLimits(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float lower = lua_tonumber(L, 2);
    float upper = lua_tonumber(L, 3);

    b2WheelJoint_SetLimits(id, lower, upper);

    return 0;
}

int wheelJointEnableMotor(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    bool enableMotor = lua_toboolean(L, 2);

    b2WheelJoint_EnableMotor(id, enableMotor);

    return 0;
}

int wheelJointIsMotorEnabled(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushboolean(L, b2WheelJoint_IsMotorEnabled(id));

    return 1;
}

int wheelJointSetMotorSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float motorSpeed = lua_tonumber(L, 2);

    b2WheelJoint_SetMotorSpeed(id, motorSpeed);

    return 0;
}

int wheelJointGetMotorSpeed(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WheelJoint_GetMotorSpeed(id));

    return 1;
}

int wheelJointSetMaxMotorTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);
    float torque = lua_tonumber(L, 2);

    b2WheelJoint_SetMaxMotorTorque(id, torque);

    return 0;
}

int wheelJointGetMaxMotorTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WheelJoint_GetMaxMotorTorque(id));

    return 1;
}

int wheelJointGetMotorTorque(lua_State* L) {
    b2JointId id = getJointIdFromLuau(L);

    lua_pushnumber(L, b2WheelJoint_GetMotorTorque(id));

    return 1;
}

static const luaL_Reg box2d_lib[] = {
    // World
    {"createWorld", createWorld},
    {"destroyWorld", destroyWorld},
    {"worldIsValid", worldIsValid},
    {"worldStep", worldStep},
    {"worldGetBodyEvents", worldGetBodyEvents},
    {"worldGetContactEvents", worldGetContactEvents},
    {"worldGetSensorEvents", worldGetSensorEvents},
    {"worldGetJointEvents", worldGetJointEvents},
    {"worldOverlapAABB", worldOverlapAABB},
    {"worldOverlapShape", worldOverlapShape},
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
    {"worldExplode", worldExplode},
    {"worldSetContactTuning", worldSetContactTuning},
    {"worldSetContactRecycleDistance", worldSetContactRecycleDistance},
    {"worldGetContactRecycleDistance", worldGetContactRecycleDistance},
    {"worldSetMaximumLinearSpeed", worldSetMaximumLinearSpeed},
    {"worldGetMaximumLinearSpeed", worldGetMaximumLinearSpeed},
    {"worldEnableWarmStarting", worldEnableWarmStarting},
    {"worldIsWarmStartingEnabled", worldIsWarmStartingEnabled},
    {"worldGetAwakeBodyCount", worldGetAwakeBodyCount},
    {"worldGetMaxCapacity", worldGetMaxCapacity},
    {"worldGetCounters", worldGetCounters},
    {"worldSetWorkerCount", worldSetWorkerCount},
    {"worldGetWorkerCount", worldGetWorkerCount},
    {"worldGetStateHash", worldGetStateHash},
    {"contactIsValid", contactIsValid},

    // Raycast
    {"worldCastRay", worldCastRay},
    {"worldCastRayClosest", worldCastRayClosest},
    {"worldCastShape", worldCastShape},
    {"worldCastMover", worldCastMover},
    {"worldCollideMover", worldCollideMover},

    // Body
    {"createBody", createBody},
    {"destroyBody", destroyBody},
    {"bodyIsValid", bodyIsValid},
    {"bodyGetType", bodyGetType},
    {"bodySetType", bodySetType},
    {"bodySetName", bodySetName},
    {"bodyGetName", bodyGetName},
    {"bodyGetPosition", bodyGetPosition},
    {"bodyGetRotation", bodyGetRotation},
    {"bodyGetTransform", bodyGetTransform},
    {"bodySetTransform", bodySetTransform},
    {"bodyGetLocalPoint", bodyGetLocalPoint},
    {"bodyGetWorldPoint", bodyGetWorldPoint},
    {"bodyGetLocalVector", bodyGetLocalVector},
    {"bodyGetWorldVector", bodyGetWorldVector},
    {"bodyGetLinearVelocity", bodyGetLinearVelocity},
    {"bodyGetAngularVelocity", bodyGetAngularVelocity},
    {"bodySetLinearVelocity", bodySetLinearVelocity},
    {"bodySetAngularVelocity", bodySetAngularVelocity},
    {"bodySetTargetTransform", bodySetTargetTransform},
    {"bodyGetLocalPointVelocity", bodyGetLocalPointVelocity},
    {"bodyGetWorldPointVelocity", bodyGetWorldPointVelocity},
    {"bodyApplyForce", bodyApplyForce},
    {"bodyApplyForceToCenter", bodyApplyForceToCenter},
    {"bodyApplyTorque", bodyApplyTorque},
    {"bodyClearForces", bodyClearForces},
    {"bodyApplyLinearImpulse", bodyApplyLinearImpulse},
    {"bodyApplyLinearImpulseToCenter", bodyApplyLinearImpulseToCenter},
    {"bodyApplyAngularImpulse", bodyApplyAngularImpulse},
    {"bodyGetMass", bodyGetMass},
    {"bodyGetRotationalInertia", bodyGetRotationalInertia},
    {"bodyGetLocalCenter", bodyGetLocalCenter},
    {"bodyGetWorldCenter", bodyGetWorldCenter},
    {"bodySetMassData", bodySetMassData},
    {"bodyGetMassData", bodyGetMassData},
    {"bodyApplyMassFromShapes", bodyApplyMassFromShapes},
    {"bodySetLinearDamping", bodySetLinearDamping},
    {"bodyGetLinearDamping", bodyGetLinearDamping},
    {"bodySetAngularDamping", bodySetAngularDamping},
    {"bodyGetAngularDamping", bodyGetAngularDamping},
    {"bodySetGravityScale", bodySetGravityScale},
    {"bodyGetGravityScale", bodyGetGravityScale},
    {"bodyIsAwake", bodyIsAwake},
    {"bodySetAwake", bodySetAwake},
    {"bodyWakeTouching", bodyWakeTouching},
    {"bodyEnableSleep", bodyEnableSleep},
    {"bodyIsSleepEnabled", bodyIsSleepEnabled},
    {"bodySetSleepThreshold", bodySetSleepThreshold},
    {"bodyGetSleepThreshold", bodyGetSleepThreshold},
    {"bodySetSafetyFactor", bodySetSafetyFactor},
    {"bodyGetSafetyFactor", bodyGetSafetyFactor},
    {"bodyIsEnabled", bodyIsEnabled},
    {"bodyDisable", bodyDisable},
    {"bodyEnable", bodyEnable},
    {"bodySetMotionLocks", bodySetMotionLocks},
    {"bodyGetMotionLocks", bodyGetMotionLocks},
    {"bodySetBullet", bodySetBullet},
    {"bodyIsBullet", bodyIsBullet},
    {"bodyEnableContactRecycling", bodyEnableContactRecycling},
    {"bodyIsContactRecyclingEnabled", bodyIsContactRecyclingEnabled},
    {"bodyEnableContactEvents", bodyEnableContactEvents},
    {"bodyEnableHitEvents", bodyEnableHitEvents},
    {"bodyGetWorld", bodyGetWorld},
    {"bodyGetShapeCount", bodyGetShapeCount},
    {"bodyGetShapes", bodyGetShapes},
    {"bodyGetJointCount", bodyGetJointCount},
    {"bodyGetJoints", bodyGetJoints},
    {"bodyGetContactCapacity", bodyGetContactCapacity},
    {"bodyComputeAABB", bodyComputeAABB},

    // Shape
    {"createCircleShape", createCircleShape},
    {"createSegmentShape", createSegmentShape},
    {"createCapsuleShape", createCapsuleShape},
    {"createPolygonShape", createPolygonShape},
    {"destroyShape", destroyShape},
    {"shapeIsValid", shapeIsValid},
    {"shapeGetType", shapeGetType},
    {"shapeGetBody", shapeGetBody},
    {"shapeGetWorld", shapeGetWorld},
    {"shapeIsSensor", shapeIsSensor},
    {"shapeSetDensity", shapeSetDensity},
    {"shapeGetDensity", shapeGetDensity},
    {"shapeSetFriction", shapeSetFriction},
    {"shapeGetFriction", shapeGetFriction},
    {"shapeSetRestitution", shapeSetRestitution},
    {"shapeGetRestitution", shapeGetRestitution},
    {"shapeSetUserMaterial", shapeSetUserMaterial},
    {"shapeGetUserMaterial", shapeGetUserMaterial},
    {"shapeSetSurfaceMaterial", shapeSetSurfaceMaterial},
    {"shapeGetSurfaceMaterial", shapeGetSurfaceMaterial},
    {"shapeGetFilter", shapeGetFilter},
    {"shapeSetFilter", shapeSetFilter},
    {"shapeEnableSensorEvents", shapeEnableSensorEvents},
    {"shapeAreSensorEventsEnabled", shapeAreSensorEventsEnabled},
    {"shapeEnableContactEvents", shapeEnableContactEvents},
    {"shapeAreContactEventsEnabled", shapeAreContactEventsEnabled},
    {"shapeEnableHitEvents", shapeEnableHitEvents},
    {"shapeAreHitEventsEnabled", shapeAreHitEventsEnabled},
    {"shapeTestPoint", shapeTestPoint},
    {"shapeGetCircle", shapeGetCircle},
    {"shapeGetSegment", shapeGetSegment},
    {"shapeGetCapsule", shapeGetCapsule},
    {"shapeGetPolygon", shapeGetPolygon},
    {"shapeSetCircle", shapeSetCircle},
    {"shapeSetCapsule", shapeSetCapsule},
    {"shapeSetSegment", shapeSetSegment},
    {"shapeSetPolygon", shapeSetPolygon},
    {"shapeGetContactCapacity", shapeGetContactCapacity},
    {"shapeGetSensorCapacity", shapeGetSensorCapacity},
    {"shapeGetAABB", shapeGetAABB},
    {"shapeComputeMassData", shapeComputeMassData},
    {"shapeGetClosestPoint", shapeGetClosestPoint},
    {"shapeApplyWind", shapeApplyWind},
    {"createChainSegmentShape", createChainSegmentShape},
    {"shapeGetChainSegment", shapeGetChainSegment},
    {"shapeSetChainSegment", shapeSetChainSegment},
    {"shapeGetParentChain", shapeGetParentChain},

    // Chain
    {"createChain", createChain},
    {"destroyChain", destroyChain},
    {"chainIsValid", chainIsValid},
    {"chainGetWorld", chainGetWorld},
    {"chainGetSegmentCount", chainGetSegmentCount},
    {"chainGetSegments", chainGetSegments},
    {"chainGetSurfaceMaterialCount", chainGetSurfaceMaterialCount},
    {"chainSetSurfaceMaterial", chainSetSurfaceMaterial},
    {"chainGetSurfaceMaterial", chainGetSurfaceMaterial},

    // Make
    {"makeOffsetRoundedBox", makeOffsetRoundedBox},
    {"makeOffsetBox", makeOffsetBox},
    {"makeRoundedBox", makeRoundedBox},
    {"makeBox", makeBox},
    {"makeSquare", makeSquare},
    {"makeOffsetRoundedPolygon", makeOffsetRoundedPolygon},
    {"makeOffsetPolygon", makeOffsetPolygon},
    {"makePolygon", makePolygon},
    {"computeHull", computeHull},

    // Joints
    {"destroyJoint", destroyJoint},
    {"jointIsValid", jointIsValid},
    {"jointGetType", jointGetType},
    {"jointGetBodyA", jointGetBodyA},
    {"jointGetBodyB", jointGetBodyB},
    {"jointGetWorld", jointGetWorld},
    {"jointSetLocalFrameA", jointSetLocalFrameA},
    {"jointGetLocalFrameA", jointGetLocalFrameA},
    {"jointSetLocalFrameB", jointSetLocalFrameB},
    {"jointGetLocalFrameB", jointGetLocalFrameB},
    {"jointSetCollideConnected", jointSetCollideConnected},
    {"jointGetCollideConnected", jointGetCollideConnected},
    {"jointWakeBodies", jointWakeBodies},
    {"jointGetConstraintForce", jointGetConstraintForce},
    {"jointGetConstraintTorque", jointGetConstraintTorque},
    {"jointGetLinearSeparation", jointGetLinearSeparation},
    {"jointGetAngularSeparation", jointGetAngularSeparation},
    {"jointSetConstraintTuning", jointSetConstraintTuning},
    {"jointGetConstraintTuning", jointGetConstraintTuning},
    {"jointSetForceThreshold", jointSetForceThreshold},
    {"jointGetForceThreshold", jointGetForceThreshold},
    {"jointSetTorqueThreshold", jointSetTorqueThreshold},
    {"jointGetTorqueThreshold", jointGetTorqueThreshold},

    // Distance joint
    {"createDistanceJoint", createDistanceJoint},
    {"distanceJointSetLength", distanceJointSetLength},
    {"distanceJointGetLength", distanceJointGetLength},
    {"distanceJointEnableSpring", distanceJointEnableSpring},
    {"distanceJointIsSpringEnabled", distanceJointIsSpringEnabled},
    {"distanceJointSetSpringForceRange", distanceJointSetSpringForceRange},
    {"distanceJointGetSpringForceRange", distanceJointGetSpringForceRange},
    {"distanceJointSetSpringHertz", distanceJointSetSpringHertz},
    {"distanceJointSetSpringDampingRatio", distanceJointSetSpringDampingRatio},
    {"distanceJointGetSpringHertz", distanceJointGetSpringHertz},
    {"distanceJointGetSpringDampingRatio", distanceJointGetSpringDampingRatio},
    {"distanceJointEnableLimit", distanceJointEnableLimit},
    {"distanceJointIsLimitEnabled", distanceJointIsLimitEnabled},
    {"distanceJointSetLengthRange", distanceJointSetLengthRange},
    {"distanceJointGetMinLength", distanceJointGetMinLength},
    {"distanceJointGetMaxLength", distanceJointGetMaxLength},
    {"distanceJointGetCurrentLength", distanceJointGetCurrentLength},
    {"distanceJointEnableMotor", distanceJointEnableMotor},
    {"distanceJointIsMotorEnabled", distanceJointIsMotorEnabled},
    {"distanceJointSetMotorSpeed", distanceJointSetMotorSpeed},
    {"distanceJointGetMotorSpeed", distanceJointGetMotorSpeed},
    {"distanceJointSetMaxMotorForce", distanceJointSetMaxMotorForce},
    {"distanceJointGetMaxMotorForce", distanceJointGetMaxMotorForce},
    {"distanceJointGetMotorForce", distanceJointGetMotorForce},

    // Filter joint
    {"createFilterJoint", createFilterJoint},

    // Motor joint
    {"createMotorJoint", createMotorJoint},
    {"motorJointSetLinearVelocity", motorJointSetLinearVelocity},
    {"motorJointGetLinearVelocity", motorJointGetLinearVelocity},
    {"motorJointSetAngularVelocity", motorJointSetAngularVelocity},
    {"motorJointGetAngularVelocity", motorJointGetAngularVelocity},
    {"motorJointSetMaxVelocityForce", motorJointSetMaxVelocityForce},
    {"motorJointGetMaxVelocityForce", motorJointGetMaxVelocityForce},
    {"motorJointSetMaxVelocityTorque", motorJointSetMaxVelocityTorque},
    {"motorJointGetMaxVelocityTorque", motorJointGetMaxVelocityTorque},
    {"motorJointSetLinearHertz", motorJointSetLinearHertz},
    {"motorJointGetLinearHertz", motorJointGetLinearHertz},
    {"motorJointSetLinearDampingRatio", motorJointSetLinearDampingRatio},
    {"motorJointGetLinearDampingRatio", motorJointGetLinearDampingRatio},
    {"motorJointSetAngularHertz", motorJointSetAngularHertz},
    {"motorJointGetAngularHertz", motorJointGetAngularHertz},
    {"motorJointSetAngularDampingRatio", motorJointSetAngularDampingRatio},
    {"motorJointGetAngularDampingRatio", motorJointGetAngularDampingRatio},
    {"motorJointSetMaxSpringForce", motorJointSetMaxSpringForce},
    {"motorJointGetMaxSpringForce", motorJointGetMaxSpringForce},
    {"motorJointSetMaxSpringTorque", motorJointSetMaxSpringTorque},
    {"motorJointGetMaxSpringTorque", motorJointGetMaxSpringTorque},

    // Mover joint
    {"createMoverJoint", createMoverJoint},
    {"moverJointSetLinearVelocity", moverJointSetLinearVelocity},
    {"moverJointGetLinearVelocity", moverJointGetLinearVelocity},
    {"moverJointSetMaxVelocityForce", moverJointSetMaxVelocityForce},
    {"moverJointGetMaxVelocityForce", moverJointGetMaxVelocityForce},

    // Pogo joint
    {"createPogoJoint", createPogoJoint},
    {"pogoJointSetRestLength", pogoJointSetRestLength},
    {"pogoJointGetRestLength", pogoJointGetRestLength},
    {"pogoJointSetSpringHertz", pogoJointSetSpringHertz},
    {"pogoJointGetSpringHertz", pogoJointGetSpringHertz},
    {"pogoJointSetSpringDampingRatio", pogoJointSetSpringDampingRatio},
    {"pogoJointGetSpringDampingRatio", pogoJointGetSpringDampingRatio},
    {"pogoJointGetLength", pogoJointGetLength},
    {"pogoJointGetVelocity", pogoJointGetVelocity},
    {"pogoJointGetImpulse", pogoJointGetImpulse},

    // Prismatic joint
    {"createPrismaticJoint", createPrismaticJoint},
    {"prismaticJointEnableSpring", prismaticJointEnableSpring},
    {"prismaticJointIsSpringEnabled", prismaticJointIsSpringEnabled},
    {"prismaticJointSetSpringHertz", prismaticJointSetSpringHertz},
    {"prismaticJointGetSpringHertz", prismaticJointGetSpringHertz},
    {"prismaticJointSetSpringDampingRatio", prismaticJointSetSpringDampingRatio},
    {"prismaticJointGetSpringDampingRatio", prismaticJointGetSpringDampingRatio},
    {"prismaticJointSetTargetTranslation", prismaticJointSetTargetTranslation},
    {"prismaticJointGetTargetTranslation", prismaticJointGetTargetTranslation},
    {"prismaticJointEnableLimit", prismaticJointEnableLimit},
    {"prismaticJointIsLimitEnabled", prismaticJointIsLimitEnabled},
    {"prismaticJointGetLowerLimit", prismaticJointGetLowerLimit},
    {"prismaticJointGetUpperLimit", prismaticJointGetUpperLimit},
    {"prismaticJointSetLimits", prismaticJointSetLimits},
    {"prismaticJointEnableMotor", prismaticJointEnableMotor},
    {"prismaticJointIsMotorEnabled", prismaticJointIsMotorEnabled},
    {"prismaticJointSetMotorSpeed", prismaticJointSetMotorSpeed},
    {"prismaticJointGetMotorSpeed", prismaticJointGetMotorSpeed},
    {"prismaticJointSetMaxMotorForce", prismaticJointSetMaxMotorForce},
    {"prismaticJointGetMaxMotorForce", prismaticJointGetMaxMotorForce},
    {"prismaticJointGetMotorForce", prismaticJointGetMotorForce},
    {"prismaticJointGetTranslation", prismaticJointGetTranslation},
    {"prismaticJointGetSpeed", prismaticJointGetSpeed},

    // Revolute joint
    {"createRevoluteJoint", createRevoluteJoint},
    {"revoluteJointEnableSpring", revoluteJointEnableSpring},
    {"revoluteJointIsSpringEnabled", revoluteJointIsSpringEnabled},
    {"revoluteJointSetSpringHertz", revoluteJointSetSpringHertz},
    {"revoluteJointGetSpringHertz", revoluteJointGetSpringHertz},
    {"revoluteJointSetSpringDampingRatio", revoluteJointSetSpringDampingRatio},
    {"revoluteJointGetSpringDampingRatio", revoluteJointGetSpringDampingRatio},
    {"revoluteJointSetTargetAngle", revoluteJointSetTargetAngle},
    {"revoluteJointGetTargetAngle", revoluteJointGetTargetAngle},
    {"revoluteJointGetAngle", revoluteJointGetAngle},
    {"revoluteJointEnableLimit", revoluteJointEnableLimit},
    {"revoluteJointIsLimitEnabled", revoluteJointIsLimitEnabled},
    {"revoluteJointGetLowerLimit", revoluteJointGetLowerLimit},
    {"revoluteJointGetUpperLimit", revoluteJointGetUpperLimit},
    {"revoluteJointSetLimits", revoluteJointSetLimits},
    {"revoluteJointEnableMotor", revoluteJointEnableMotor},
    {"revoluteJointIsMotorEnabled", revoluteJointIsMotorEnabled},
    {"revoluteJointSetMotorSpeed", revoluteJointSetMotorSpeed},
    {"revoluteJointGetMotorSpeed", revoluteJointGetMotorSpeed},
    {"revoluteJointGetMotorTorque", revoluteJointGetMotorTorque},
    {"revoluteJointSetMaxMotorTorque", revoluteJointSetMaxMotorTorque},
    {"revoluteJointGetMaxMotorTorque", revoluteJointGetMaxMotorTorque},

    // Weld joint
    {"createWeldJoint", createWeldJoint},
    {"weldJointSetLinearHertz", weldJointSetLinearHertz},
    {"weldJointGetLinearHertz", weldJointGetLinearHertz},
    {"weldJointSetLinearDampingRatio", weldJointSetLinearDampingRatio},
    {"weldJointGetLinearDampingRatio", weldJointGetLinearDampingRatio},
    {"weldJointSetAngularHertz", weldJointSetAngularHertz},
    {"weldJointGetAngularHertz", weldJointGetAngularHertz},
    {"weldJointSetAngularDampingRatio", weldJointSetAngularDampingRatio},
    {"weldJointGetAngularDampingRatio", weldJointGetAngularDampingRatio},

    // Wheel joint
    {"createWheelJoint", createWheelJoint},
    {"wheelJointEnableSpring", wheelJointEnableSpring},
    {"wheelJointIsSpringEnabled", wheelJointIsSpringEnabled},
    {"wheelJointSetSpringHertz", wheelJointSetSpringHertz},
    {"wheelJointGetSpringHertz", wheelJointGetSpringHertz},
    {"wheelJointSetSpringDampingRatio", wheelJointSetSpringDampingRatio},
    {"wheelJointGetSpringDampingRatio", wheelJointGetSpringDampingRatio},
    {"wheelJointEnableLimit", wheelJointEnableLimit},
    {"wheelJointIsLimitEnabled", wheelJointIsLimitEnabled},
    {"wheelJointGetLowerLimit", wheelJointGetLowerLimit},
    {"wheelJointGetUpperLimit", wheelJointGetUpperLimit},
    {"wheelJointSetLimits", wheelJointSetLimits},
    {"wheelJointEnableMotor", wheelJointEnableMotor},
    {"wheelJointIsMotorEnabled", wheelJointIsMotorEnabled},
    {"wheelJointSetMotorSpeed", wheelJointSetMotorSpeed},
    {"wheelJointGetMotorSpeed", wheelJointGetMotorSpeed},
    {"wheelJointSetMaxMotorTorque", wheelJointSetMaxMotorTorque},
    {"wheelJointGetMaxMotorTorque", wheelJointGetMaxMotorTorque},
    {"wheelJointGetMotorTorque", wheelJointGetMotorTorque},

    {nullptr, nullptr},
};

void registerBox2dFunctions(lua_State* L, ResourceState* state) {
    luaL_register(L, "box2d", box2d_lib);
    lua_setreadonly(L, -1, 1);
}