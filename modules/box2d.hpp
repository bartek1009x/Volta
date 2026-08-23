#ifndef BOX2D_BINDINGS_H
#define BOX2D_BINDINGS_H

#include "../dependencies/luau/VM/include/lua.h"
#include "box2d/box2d.h"
#include "../utils/ResourceState.hpp"

// Helpers
b2WorldDef constructWorldDef(lua_State* L, int defTableIndex);
b2QueryFilter constructQueryFilter(lua_State* L, int index);
b2BodyDef constructBodyDef(lua_State* L, int defTableIndex);
b2Segment constructSegment(lua_State* L, int index);
b2Filter constructFilter(lua_State* L, int index);
b2Circle constructCircle(lua_State* L, int index);
b2Capsule constructCapsule(lua_State* L, int index);
b2Polygon constructPolygon(lua_State* L, int index);
b2ShapeDef constructShapeDef(lua_State* L, int defTableIndex);
b2Hull constructHull(lua_State* L, int index);
b2QueryFilter constructQueryFilter(lua_State* L, int index);
b2ShapeProxy constructShapeProxy(lua_State* L, int index);

void applyVec2(lua_State* L, int index, b2Vec2& value);
void applySurfaceMaterial(lua_State* L, int index, b2SurfaceMaterial& material);
void applyFilter(lua_State* L, int index, b2Filter& filter);

void pushWorldId(lua_State* L, b2WorldId id);
void pushBodyId(lua_State* L, b2BodyId id);
void pushMassData(lua_State* L, b2MassData data);
void pushMotionLocks(lua_State* L, b2MotionLocks locks);
void pushShapeId(lua_State* L, b2ShapeId id);
void pushVec2(lua_State* L, b2Vec2 value);
void pushSurfaceMaterial(lua_State* L, b2SurfaceMaterial material);
void pushSegment(lua_State* L, b2Segment segment);
void pushFilter(lua_State* L, b2Filter filter);
void pushCircle(lua_State* L, b2Circle circle);
void pushCapsule(lua_State* L, b2Capsule capsule);
void pushContactId(lua_State* L, b2ContactId id);
void pushTreeStats(lua_State* L, b2TreeStats stats);
void pushRayResult(lua_State* L, b2RayResult result);
void pushPlane(lua_State* L, b2Plane plane);
void pushPlaneResult(lua_State* L, const b2PlaneResult& result);
bool planeResultCallback(b2ShapeId shapeId, const b2PlaneResult* plane, void* context);

// World
int createWorld(lua_State* L);
int destroyWorld(lua_State* L);
int worldIsValid(lua_State* L);
int worldStep(lua_State* L);
int worldGetBounds(lua_State* L);
int worldGetBodyEvents(lua_State* L);
int worldGetContactEvents(lua_State* L);
int worldGetSensorEvents(lua_State* L);
int worldOverlapAABB(lua_State* L);
int worldOverlapShape(lua_State* L);
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
int contactIsValid(lua_State* L);

// Raycast
int worldCastRay(lua_State* L);
int worldCastRayClosest(lua_State* L);
int worldCastShape(lua_State* L);
int worldCastMover(lua_State* L);
int worldCollideMover(lua_State* L);

// Body
int createBody(lua_State* L);
int destroyBody(lua_State* L);
int bodyIsValid(lua_State* L);
int bodyGetType(lua_State* L);
int bodySetType(lua_State* L);
int bodySetName(lua_State* L);
int bodyGetName(lua_State* L);
int bodyGetPosition(lua_State* L);
int bodyGetRotation(lua_State* L);
int bodyGetTransform(lua_State* L);
int bodySetTransform(lua_State* L);
int bodyGetLocalPoint(lua_State* L);
int bodyGetWorldPoint(lua_State* L);
int bodyGetLocalVector(lua_State* L);
int bodyGetWorldVector(lua_State* L);
int bodyGetLinearVelocity(lua_State* L);
int bodyGetAngularVelocity(lua_State* L);
int bodySetLinearVelocity(lua_State* L);
int bodySetAngularVelocity(lua_State* L);
int bodySetTargetTransform(lua_State* L);
int bodyGetLocalPointVelocity(lua_State* L);
int bodyGetWorldPointVelocity(lua_State* L);
int bodyApplyForce(lua_State* L);
int bodyApplyForceToCenter(lua_State* L);
int bodyApplyTorque(lua_State* L);
int bodyClearForces(lua_State* L);
int bodyApplyLinearImpulse(lua_State* L);
int bodyApplyLinearImpulseToCenter(lua_State* L);
int bodyApplyAngularImpulse(lua_State* L);
int bodyGetMass(lua_State* L);
int bodyGetRotationalInertia(lua_State* L);
int bodyGetLocalCenter(lua_State* L);
int bodyGetWorldCenter(lua_State* L);
int bodySetMassData(lua_State* L);
int bodyGetMassData(lua_State* L);
int bodyApplyMassFromShapes(lua_State* L);
int bodySetLinearDamping(lua_State* L);
int bodyGetLinearDamping(lua_State* L);
int bodySetAngularDamping(lua_State* L);
int bodyGetAngularDamping(lua_State* L);
int bodySetGravityScale(lua_State* L);
int bodyGetGravityScale(lua_State* L);
int bodyIsAwake(lua_State* L);
int bodySetAwake(lua_State* L);
int bodyWakeTouching(lua_State* L);
int bodyEnableSleep(lua_State* L);
int bodyIsSleepEnabled(lua_State* L);
int bodySetSleepThreshold(lua_State* L);
int bodyGetSleepThreshold(lua_State* L);
int bodySetSafetyFactor(lua_State* L);
int bodyGetSafetyFactor(lua_State* L);
int bodyIsEnabled(lua_State* L);
int bodyDisable(lua_State* L);
int bodyEnable(lua_State* L);
int bodySetMotionLocks(lua_State* L);
int bodyGetMotionLocks(lua_State* L);
int bodySetBullet(lua_State* L);
int bodyIsBullet(lua_State* L);
int bodyEnableContactRecycling(lua_State* L);
int bodyIsContactRecyclingEnabled(lua_State* L);
int bodyEnableContactEvents(lua_State* L);
int bodyEnableHitEvents(lua_State* L);
int bodyGetWorld(lua_State* L);
int bodyGetShapeCount(lua_State* L);
int bodyGetJointCount(lua_State* L);
int bodyGetContactCapacity(lua_State* L);
int bodyComputeAABB(lua_State* L);

// Shape
int createCircleShape(lua_State* L);
int createSegmentShape(lua_State* L);
int createCapsuleShape(lua_State* L);
int createPolygonShape(lua_State* L);
int destroyShape(lua_State* L);
int shapeIsValid(lua_State* L);
int shapeGetType(lua_State* L);
int shapeGetBody(lua_State* L);
int shapeGetWorld(lua_State* L);
int shapeIsSensor(lua_State* L);
int shapeSetDensity(lua_State* L);
int shapeGetDensity(lua_State* L);
int shapeSetFriction(lua_State* L);
int shapeGetFriction(lua_State* L);
int shapeSetRestitution(lua_State* L);
int shapeGetRestitution(lua_State* L);
int shapeSetUserMaterial(lua_State* L);
int shapeGetUserMaterial(lua_State* L);
int shapeSetSurfaceMaterial(lua_State* L);
int shapeGetSurfaceMaterial(lua_State* L);
int shapeGetFilter(lua_State* L);
int shapeSetFilter(lua_State* L);
int shapeEnableSensorEvents(lua_State* L);
int shapeAreSensorEventsEnabled(lua_State* L);
int shapeEnableContactEvents(lua_State* L);
int shapeAreContactEventsEnabled(lua_State* L);
int shapeEnableHitEvents(lua_State* L);
int shapeAreHitEventsEnabled(lua_State* L);
int shapeTestPoint(lua_State* L);
int shapeGetCircle(lua_State* L);
int shapeGetSegment(lua_State* L);
int shapeGetCapsule(lua_State* L);
int shapeGetPolygon(lua_State* L);
int shapeSetCircle(lua_State* L);
int shapeSetCapsule(lua_State* L);
int shapeSetSegment(lua_State* L);
int shapeSetPolygon(lua_State* L);
int shapeGetContactCapacity(lua_State* L);
int shapeGetSensorCapacity(lua_State* L);
int shapeGetAABB(lua_State* L);
int shapeComputeMassData(lua_State* L);
int shapeGetClosestPoint(lua_State* L);
int shapeApplyWind(lua_State* L);

// Make
int makeOffsetRoundedBox(lua_State* L);
int makeOffsetBox(lua_State* L);
int makeRoundedBox(lua_State* L);
int makeBox(lua_State* L);
int makeSquare(lua_State* L);
int makeOffsetRoundedPolygon(lua_State* L);
int makeOffsetPolygon(lua_State* L);
int makePolygon(lua_State* L);

void registerBox2dFunctions(lua_State* L, ResourceState* state);

#endif