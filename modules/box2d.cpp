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

b2WorldId getWorldIdFromLuau(lua_State* L) {
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

void pushWorldId(lua_State* L, b2WorldId id) {
    lua_createtable(L, 2, 0);

    lua_pushinteger(L, id.index1);
    lua_rawseti(L, -2, 1);

    lua_pushinteger(L, id.generation);
    lua_rawseti(L, -2, 2);
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
    uint64_t hash = b2World_GetStateHash(id);

    lua_pushinteger64(L, hash);

    return 1;
}

// Body

b2BodyId getBodyIdFromLuau(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_argerror(L, 1, "The body ID must be a table");
        return {};
    }

    lua_rawgeti(L, 1, 1);
    int index1 = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_rawgeti(L, 1, 2);
    Uint16 world0 = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_rawgeti(L, 1, 3);
    Uint16 generation = lua_tointeger(L, -1);
    lua_pop(L, 1);

    b2BodyId id = {index1, world0, generation};

    return id;
}

void pushBodyId(lua_State* L, b2BodyId id) {
    lua_createtable(L, 3, 0);

    lua_pushinteger(L, id.index1);
    lua_rawseti(L, -2, 1);

    lua_pushinteger(L, id.world0);
    lua_rawseti(L, -2, 2);

    lua_pushinteger(L, id.generation);
    lua_rawseti(L, -2, 3);
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

    lua_pushnil(L);
    while (lua_next(L, defTableIndex) != 0) {
        const char* field = lua_tostring(L, -2);

        if (strcmp(field, "type") == 0) {
            def.type = static_cast<b2BodyType>(lua_tointeger(L, -1));
        }
        else if (strcmp(field, "position") == 0) {
            int positionIndex = lua_gettop(L);

            lua_pushnil(L);
            while (lua_next(L, positionIndex) != 0) {
                const char* positionField = lua_tostring(L, -2);

                if (strcmp(positionField, "x") == 0) {
                    def.position.x = lua_tonumber(L, -1);
                }
                else if (strcmp(positionField, "y") == 0) {
                    def.position.y = lua_tonumber(L, -1);
                }

                lua_pop(L, 1);
            }
        }
        else if (strcmp(field, "rotation") == 0) {
            int rotationIndex = lua_gettop(L);

            lua_pushnil(L);
            while (lua_next(L, rotationIndex) != 0) {
                const char* rotationField = lua_tostring(L, -2);

                if (strcmp(rotationField, "c") == 0) {
                    def.rotation.c = lua_tonumber(L, -1);
                }
                else if (strcmp(rotationField, "s") == 0) {
                    def.rotation.s = lua_tonumber(L, -1);
                }

                lua_pop(L, 1);
            }
        }
        else if (strcmp(field, "linearVelocity") == 0) {
            int velocityIndex = lua_gettop(L);

            lua_pushnil(L);
            while (lua_next(L, velocityIndex) != 0) {
                const char* velocityField = lua_tostring(L, -2);

                if (strcmp(velocityField, "x") == 0) {
                    def.linearVelocity.x = lua_tonumber(L, -1);
                }
                else if (strcmp(velocityField, "y") == 0) {
                    def.linearVelocity.y = lua_tonumber(L, -1);
                }

                lua_pop(L, 1);
            }
        }
        else if (strcmp(field, "angularVelocity") == 0) {
            def.angularVelocity = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "linearDamping") == 0) {
            def.linearDamping = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "angularDamping") == 0) {
            def.angularDamping = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "gravityScale") == 0) {
            def.gravityScale = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "sleepThreshold") == 0) {
            def.sleepThreshold = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "safetyFactor") == 0) {
            def.safetyFactor = lua_tonumber(L, -1);
        }
        else if (strcmp(field, "name") == 0) {
            def.name = lua_tostring(L, -1);
        }
        else if (strcmp(field, "motionLocks") == 0) {
            int motionLocksIndex = lua_gettop(L);

            lua_pushnil(L);
            while (lua_next(L, motionLocksIndex) != 0) {
                const char* motionLocksField = lua_tostring(L, -2);

                if (strcmp(motionLocksField, "linearX") == 0) {
                    def.motionLocks.linearX = lua_toboolean(L, -1);
                }
                else if (strcmp(motionLocksField, "linearY") == 0) {
                    def.motionLocks.linearY = lua_toboolean(L, -1);
                }
                else if (strcmp(motionLocksField, "angularZ") == 0) {
                    def.motionLocks.angularZ = lua_toboolean(L, -1);
                }

                lua_pop(L, 1);
            }
        }
        else if (strcmp(field, "enableSleep") == 0) {
            def.enableSleep = lua_toboolean(L, -1);
        }
        else if (strcmp(field, "isAwake") == 0) {
            def.isAwake = lua_toboolean(L, -1);
        }
        else if (strcmp(field, "isBullet") == 0) {
            def.isBullet = lua_toboolean(L, -1);
        }
        else if (strcmp(field, "isEnabled") == 0) {
            def.isEnabled = lua_toboolean(L, -1);
        }
        else if (strcmp(field, "allowFastRotation") == 0) {
            def.allowFastRotation = lua_toboolean(L, -1);
        }
        else if (strcmp(field, "enableContactRecycling") == 0) {
            def.enableContactRecycling = lua_toboolean(L, -1);
        }

        lua_pop(L, 1);
    }

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

int bodyGetJointCount(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    lua_pushinteger(L, b2Body_GetJointCount(id));

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
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_argerror(L, 1, "The shape ID must be a table");
        return {};
    }

    lua_rawgeti(L, 1, 1);
    int index1 = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_rawgeti(L, 1, 2);
    Uint16 world0 = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_rawgeti(L, 1, 3);
    Uint16 generation = lua_tointeger(L, -1);
    lua_pop(L, 1);

    b2ShapeId id = {};
    id.index1 = index1;
    id.world0 = world0;
    id.generation = generation;

    return id;
}

void pushShapeId(lua_State* L, b2ShapeId id) {
    lua_createtable(L, 3, 0);

    lua_pushinteger(L, id.index1);
    lua_rawseti(L, -2, 1);

    lua_pushinteger(L, id.world0);
    lua_rawseti(L, -2, 2);

    lua_pushinteger(L, id.generation);
    lua_rawseti(L, -2, 3);
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
    b2Filter filter = {};

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

    uint64_t material = lua_tointeger64(L, 2, nullptr);

    b2Shape_SetUserMaterial(id, material);

    return 0;
}

int shapeGetUserMaterial(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    uint64_t material = b2Shape_GetUserMaterial(id);

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

int shapeEnablePreSolveEvents(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);
    bool flag = lua_toboolean(L, 2);

    b2Shape_EnablePreSolveEvents(id, flag);

    return 0;
}

int shapeArePreSolveEventsEnabled(lua_State* L) {
    b2ShapeId id = getShapeIdFromLuau(L);

    lua_pushboolean(L, b2Shape_ArePreSolveEventsEnabled(id));

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

int makeOffsetRoundedBox(lua_State* L) {
    float halfWidth = lua_tonumber(L, 1);
    float halfHeight = lua_tonumber(L, 2);

    b2Vec2 center = {
        static_cast<float>(lua_tonumber(L, 3)),
        static_cast<float>(lua_tonumber(L, 4))
    };

    b2Rot rotation = {
        static_cast<float>(lua_tonumber(L, 5)),
        static_cast<float>(lua_tonumber(L, 6))
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
        static_cast<float>(lua_tonumber(L, 3)),
        static_cast<float>(lua_tonumber(L, 4))
    };

    b2Rot rotation = {
        static_cast<float>(lua_tonumber(L, 5)),
        static_cast<float>(lua_tonumber(L, 6))
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
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Rot rotation = {
        static_cast<float>(lua_tonumber(L, 4)),
        static_cast<float>(lua_tonumber(L, 5))
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
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Rot rotation = {
        static_cast<float>(lua_tonumber(L, 4)),
        static_cast<float>(lua_tonumber(L, 5))
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

static const luaL_Reg box2d_lib[] = {
    // World
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
    {"bodyGetJointCount", bodyGetJointCount},
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
    {"shapeEnablePreSolveEvents", shapeEnablePreSolveEvents},
    {"shapeArePreSolveEventsEnabled", shapeArePreSolveEventsEnabled},
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

    // Make
    {"makeOffsetRoundedBox", makeOffsetRoundedBox},
    {"makeOffsetBox", makeOffsetBox},
    {"makeRoundedBox", makeRoundedBox},
    {"makeBox", makeBox},
    {"makeSquare", makeSquare},
    {"makeOffsetRoundedPolygon", makeOffsetRoundedPolygon},
    {"makeOffsetPolygon", makeOffsetPolygon},
    {"makePolygon", makePolygon},

    {nullptr, nullptr},
};

void registerBox2dFunctions(lua_State* L, ResourceState* state) {
    luaL_register(L, "box2d", box2d_lib);
    lua_setreadonly(L, -1, 1);
}