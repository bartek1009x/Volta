#include "box2d.hpp"

#include "../dependencies/luau/VM/include/lualib.h"
#include "box2d/box2d.h"
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

    lua_pushinteger64(L, static_cast<int64_t>(hash));

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
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Rot rotation = {
        static_cast<float>(lua_tonumber(L, 4)),
        static_cast<float>(lua_tonumber(L, 5))
    };

    b2Body_SetTransform(id, position, rotation);

    return 0;
}

int bodyGetLocalPoint(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Pos worldPoint = {
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Vec2 point = b2Body_GetLocalPoint(id, worldPoint);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int bodyGetWorldPoint(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 localPoint = {
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Pos point = b2Body_GetWorldPoint(id, localPoint);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int bodyGetLocalVector(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 worldVector = {
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Vec2 vector = b2Body_GetLocalVector(id, worldVector);

    lua_pushnumber(L, vector.x);
    lua_pushnumber(L, vector.y);

    return 2;
}

int bodyGetWorldVector(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 localVector = {
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
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
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
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
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Vec2 velocity = b2Body_GetLocalPointVelocity(id, localPoint);

    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);

    return 2;
}

int bodyGetWorldPointVelocity(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Pos worldPoint = {
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Vec2 velocity = b2Body_GetWorldPointVelocity(id, worldPoint);

    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);

    return 2;
}

int bodyApplyForce(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 force = {
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Pos point = {
        static_cast<float>(lua_tonumber(L, 4)),
        static_cast<float>(lua_tonumber(L, 5))
    };

    bool wake = lua_toboolean(L, 6);

    b2Body_ApplyForce(id, force, point, wake);

    return 0;
}

int bodyApplyForceToCenter(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 force = {
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
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
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
    };

    b2Pos point = {
        static_cast<float>(lua_tonumber(L, 4)),
        static_cast<float>(lua_tonumber(L, 5))
    };

    bool wake = lua_toboolean(L, 6);

    b2Body_ApplyLinearImpulse(id, impulse, point, wake);

    return 0;
}

int bodyApplyLinearImpulseToCenter(lua_State* L) {
    b2BodyId id = getBodyIdFromLuau(L);

    b2Vec2 impulse = {
        static_cast<float>(lua_tonumber(L, 2)),
        static_cast<float>(lua_tonumber(L, 3))
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

    {nullptr, nullptr},
};

void registerBox2dFunctions(lua_State* L, ResourceState* state) {
    luaL_register(L, "box2d", box2d_lib);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "box2d");
}