--!strict

local box2d = {}

export type vec2 = {
    x: number,
    y: number
}
export type worldId = {number}
export type capacity = {
    staticShapeCount: number,
    dynamicShapeCount: number,
    staticBodyCount: number,
    dynamicBodyCount: number,
    contactCount: number
}
export type worldDef = {
    gravity: vec2?,
    restitutionThreshold: number?,
    hitEventThreshold: number?,
    contactHertz: number?,
    contactDampingRatio: number?,
    contactSpeed: number?,
    maximumLinearSpeed: number?,
    enableSleep: boolean?,
    enableContinuous: boolean?,
    enableContactSoftening: boolean?,
    workerCount: number?,
    capacity: capacity?
}
export type counters = {
    byteCount: number,

   	bodyCount: number,
	shapeCount: number,
	contactCount: number,
	jointCount: number,
	islandCount: number,
	stackUsed: number,
	staticTreeHeight: number,
	treeHeight: number,
	taskCount: number,
	colorCounts: {number},

	awakeContactCount: number,

	recycledContactCount: number
}

export type bodyId = {number}
export type bodyType = number
export type rot = {
    c: number,
    s: number
}
export type motionLocks = {
    linearX: boolean,
    linearY: boolean,
    angularZ: boolean
}
export type massData = {
    mass: number,
    center: vec2,
    rotationalInertia: number
}
export type bodyDef = {
    type: bodyType?,
    position: vec2?,
    rotation: rot?,
    linearVelocity: vec2?,
    angularVelocity: number?,
    linearDamping: number?,
    angularDamping: number?,
    gravityScale: number?,
    sleepThreshold: number?,
    safetyFactor: number?,
    name: string?,
    motionLocks: motionLocks?,
    enableSleep: boolean?,
    isAwake: boolean?,
    isBullet: boolean?,
    isEnabled: boolean?,
    allowFastRotation: boolean?,
    enableContactRecycling: boolean?
}

export type shapeId = {number}
export type shapeType = number
export type surfaceMaterial = {
    friction: number,
    restitution: number,
    rollingResistance: number,
    tangentSpeed: number,
    userMaterialId: number,
    customColor: number,
}
export type surfaceMaterialDef = {
    friction: number?,
    restitution: number?,
    rollingResistance: number?,
    tangentSpeed: number?,
    userMaterialId: number?,
    customColor: number?,
}
export type filter = {
    categoryBits: number,
    maskBits: number,
    groupIndex: number,
}
export type filterDef = {
    categoryBits: number?,
    maskBits: number?,
    groupIndex: number?,
}
export type shapeDef = {
    material: surfaceMaterialDef?,
    density: number?,
    filter: filterDef?,
    enableCustomFiltering: boolean?,
    isSensor: boolean?,
    enableSensorEvents: boolean?,
    enableContactEvents: boolean?,
    enableHitEvents: boolean?,
    enablePreSolveEvents: boolean?,
    invokeContactCreation: boolean?,
    updateBodyMass: boolean?,
}
export type circle = {
    center: vec2,
    radius: number,
}
export type segment = {
    point1: vec2,
    point2: vec2,
}
export type capsule = {
    center1: vec2,
    center2: vec2,
    radius: number,
}
export type polygon = {
    vertices: {vec2},
    normals: {vec2},
    centroid: vec2,
    radius: number,
    count: number,
}

export type hull = {
    points: {vec2},
    count: number,
}

export type transform = {
    p: vec2,
    q: rot,
}
export type bodyMoveEvent = {
    transform: transform,
    bodyId: bodyId,
    fellAsleep: boolean,
}
export type bodyEvents = {bodyMoveEvent}
export type contactId = {number}
export type contactBeginTouchEvent = {
    shapeIdA: shapeId,
    shapeIdB: shapeId,
    contactId: contactId,
}
export type contactEndTouchEvent = {
    shapeIdA: shapeId,
    shapeIdB: shapeId,
    contactId: contactId,
}
export type contactHitEvent = {
    shapeIdA: shapeId,
    shapeIdB: shapeId,
    contactId: contactId,
    point: vec2,
    normal: vec2,
    approachSpeed: number,
}
export type contactEvents = {
    beginEvents: {contactBeginTouchEvent},
    endEvents: {contactEndTouchEvent},
    hitEvents: {contactHitEvent},
    beginCount: number,
    endCount: number,
    hitCount: number,
}
export type sensorBeginTouchEvent = {
    sensorShapeId: shapeId,
    visitorShapeId: shapeId,
}
export type sensorEndTouchEvent = {
    sensorShapeId: shapeId,
    visitorShapeId: shapeId,
}
export type sensorEvents = {
    beginEvents: {sensorBeginTouchEvent},
    endEvents: {sensorEndTouchEvent},
    beginCount: number,
    endCount: number,
}

-- World

function box2d.createWorld(worldDef: worldDef) : worldId
    return {0, 0}
end
function box2d.destroyWorld(worldId: worldId) end
function box2d.worldIsValid(worldDef: worldDef) : boolean
    return false
end
function box2d.worldStep(worldId: worldId, timeStep: number, subStepCount: number) end
function box2d.worldGetBounds(worldId: worldId) : number
    return 0, 0, 0, 0
end
function box2d.worldGetBodyEvents(worldId: worldId): bodyEvents
    return {} :: bodyEvents
end
function box2d.worldGetContactEvents(worldId: worldId): contactEvents
    return {} :: contactEvents
end
function box2d.worldGetSensorEvents(worldId: worldId): sensorEvents
    return {} :: sensorEvents
end
function box2d.worldEnableSleeping(worldId: worldId, flag: boolean) end
function box2d.worldIsSleepingEnabled(worldId: worldId) : boolean
    return false
end
function box2d.worldEnableContinuous(worldId: worldId, flag: boolean) end
function box2d.worldIsContinuousEnabled(worldId: worldId) : boolean
    return false
end
function box2d.worldSetRestitutionThreshold(worldId: worldId, value: number) end
function box2d.worldGetRestitutionThreshold(worldId: worldId) : number
    return 0
end
function box2d.worldSetHitEventThreshold(worldId: worldId, value: number) end
function box2d.worldGetHitEventThreshold(worldId: worldId) : number
    return 0
end
function box2d.worldSetGravity(worldId: worldId, gravityX: number, gravityY: number) end
function box2d.worldGetGravity(worldId: worldId) : number
    return 0, 0
end
function box2d.worldSetContactTuning(worldId: worldId, hertz: number, dampingRatio: number, pushSpeed: number) end
function box2d.worldSetContactRecycleDistance(worldId: worldId, recycleDistance: number) end
function box2d.worldGetContactRecycleDistance(worldId: worldId) : number
    return 0
end
function box2d.worldSetMaximumLinearSpeed(worldId: worldId, maximumLinearSpeed: number) end
function box2d.worldGetMaximumLinearSpeed(worldId: worldId) : number
    return 0
end
function box2d.worldEnableWarmStarting(worldId: worldId, flag: boolean) end
function box2d.worldIsWarmStartingEnabled(worldId: worldId) : boolean
    return false
end
function box2d.worldGetAwakeBodyCount(worldId: worldId) : number
    return 0
end
function box2d.worldGetCounters(worldId: worldId) : counters
    return {} :: counters
end
function box2d.worldGetMaxCapacity(worldId: worldId) : capacity
    return {} :: capacity
end
function box2d.worldSetWorkerCount(worldId: worldId, count: number) end
function box2d.worldGetWorkerCount(worldId: worldId) : number
    return 0
end
function box2d.worldGetStateHash(worldId: worldId) : number
    return 0
end
function box2d.contactIsValid(contactId: contactId): boolean
    return false
end

-- Body

function box2d.createBody(worldId: worldId, bodyDef: bodyDef): bodyId
    return {0, 0, 0}
end
function box2d.destroyBody(bodyId: bodyId) end
function box2d.bodyIsValid(bodyId: bodyId): boolean
    return false
end
function box2d.bodyGetType(bodyId: bodyId): bodyType
    return 0
end
function box2d.bodySetType(bodyId: bodyId, bodyType: bodyType) end
function box2d.bodySetName(bodyId: bodyId, name: string) end
function box2d.bodyGetName(bodyId: bodyId): string?
    return nil
end
function box2d.bodyGetPosition(bodyId: bodyId): (number, number)
    return 0, 0
end
function box2d.bodyGetRotation(bodyId: bodyId): (number, number)
    return 0, 0
end
function box2d.bodyGetTransform(bodyId: bodyId): (number, number, number, number)
    return 0, 0, 0, 0
end
function box2d.bodySetTransform(bodyId: bodyId, positionX: number, positionY: number, rotationC: number, rotationS: number) end
function box2d.bodyGetLocalPoint(bodyId: bodyId, worldPointX: number, worldPointY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetWorldPoint(bodyId: bodyId, localPointX: number, localPointY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetLocalVector(bodyId: bodyId, worldVectorX: number, worldVectorY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetWorldVector(bodyId: bodyId, localVectorX: number, localVectorY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetLinearVelocity(bodyId: bodyId): (number, number)
    return 0, 0
end
function box2d.bodyGetAngularVelocity(bodyId: bodyId): number
    return 0
end
function box2d.bodySetLinearVelocity(bodyId: bodyId, velocityX: number, velocityY: number) end
function box2d.bodySetAngularVelocity(bodyId: bodyId, angularVelocity: number) end
function box2d.bodySetTargetTransform(bodyId: bodyId, positionX: number, positionY: number, rotationC: number, rotationS: number, timeStep: number, wake: boolean) end
function box2d.bodyGetLocalPointVelocity(bodyId: bodyId, localPointX: number, localPointY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetWorldPointVelocity(bodyId: bodyId, worldPointX: number, worldPointY: number): (number, number)
    return 0, 0
end
function box2d.bodyApplyForce(bodyId: bodyId, forceX: number, forceY: number, pointX: number, pointY: number, wake: boolean) end
function box2d.bodyApplyForceToCenter(bodyId: bodyId, forceX: number, forceY: number, wake: boolean) end
function box2d.bodyApplyTorque(bodyId: bodyId, torque: number, wake: boolean) end
function box2d.bodyClearForces(bodyId: bodyId) end
function box2d.bodyApplyLinearImpulse(bodyId: bodyId, impulseX: number, impulseY: number, pointX: number, pointY: number, wake: boolean) end
function box2d.bodyApplyLinearImpulseToCenter(bodyId: bodyId, impulseX: number, impulseY: number, wake: boolean) end
function box2d.bodyApplyAngularImpulse(bodyId: bodyId, impulse: number, wake: boolean) end
function box2d.bodyGetMass(bodyId: bodyId): number
    return 0
end
function box2d.bodyGetRotationalInertia(bodyId: bodyId): number
    return 0
end
function box2d.bodyGetLocalCenter(bodyId: bodyId): (number, number)
    return 0, 0
end
function box2d.bodyGetWorldCenter(bodyId: bodyId): (number, number)
    return 0, 0
end
function box2d.bodySetMassData(bodyId: bodyId, massData: massData) end
function box2d.bodyGetMassData(bodyId: bodyId): massData
    return {} :: massData
end
function box2d.bodyApplyMassFromShapes(bodyId: bodyId) end
function box2d.bodySetLinearDamping(bodyId: bodyId, linearDamping: number) end
function box2d.bodyGetLinearDamping(bodyId: bodyId): number
    return 0
end
function box2d.bodySetAngularDamping(bodyId: bodyId, angularDamping: number) end
function box2d.bodyGetAngularDamping(bodyId: bodyId): number
    return 0
end
function box2d.bodySetGravityScale(bodyId: bodyId, gravityScale: number) end
function box2d.bodyGetGravityScale(bodyId: bodyId): number
    return 0
end
function box2d.bodyIsAwake(bodyId: bodyId): boolean
    return false
end
function box2d.bodySetAwake(bodyId: bodyId, awake: boolean) end
function box2d.bodyWakeTouching(bodyId: bodyId) end
function box2d.bodyEnableSleep(bodyId: bodyId, enableSleep: boolean) end
function box2d.bodyIsSleepEnabled(bodyId: bodyId): boolean
    return false
end
function box2d.bodySetSleepThreshold(bodyId: bodyId, sleepThreshold: number) end
function box2d.bodyGetSleepThreshold(bodyId: bodyId): number
    return 0
end
function box2d.bodySetSafetyFactor(bodyId: bodyId, safetyFactor: number) end
function box2d.bodyGetSafetyFactor(bodyId: bodyId): number
    return 0
end
function box2d.bodyIsEnabled(bodyId: bodyId): boolean
    return false
end
function box2d.bodyDisable(bodyId: bodyId) end
function box2d.bodyEnable(bodyId: bodyId) end
function box2d.bodySetMotionLocks(bodyId: bodyId, locks: motionLocks) end
function box2d.bodyGetMotionLocks(bodyId: bodyId): motionLocks
    return {} :: motionLocks
end
function box2d.bodySetBullet(bodyId: bodyId, flag: boolean) end
function box2d.bodyIsBullet(bodyId: bodyId): boolean
    return false
end
function box2d.bodyEnableContactRecycling(bodyId: bodyId, flag: boolean) end
function box2d.bodyIsContactRecyclingEnabled(bodyId: bodyId): boolean
    return false
end
function box2d.bodyEnableContactEvents(bodyId: bodyId, flag: boolean) end
function box2d.bodyEnableHitEvents(bodyId: bodyId, flag: boolean)
end
function box2d.bodyGetWorld(bodyId: bodyId): worldId
    return {0, 0}
end
function box2d.bodyGetShapeCount(bodyId: bodyId): number
    return 0
end
function box2d.bodyGetJointCount(bodyId: bodyId): number
    return 0
end
function box2d.bodyGetContactCapacity(bodyId: bodyId): number
    return 0
end
function box2d.bodyComputeAABB(bodyId: bodyId): (number, number, number, number)
    return 0, 0, 0, 0
end

-- Shape

function box2d.createCircleShape(bodyId: bodyId, shapeDef: shapeDef, circle: circle): shapeId
    return {0, 0, 0}
end
function box2d.createSegmentShape(bodyId: bodyId, shapeDef: shapeDef, segment: segment): shapeId
    return {0, 0, 0}
end
function box2d.createCapsuleShape(bodyId: bodyId, shapeDef: shapeDef, capsule: capsule): shapeId
    return {0, 0, 0}
end
function box2d.createPolygonShape(bodyId: bodyId, shapeDef: shapeDef, polygon: polygon): shapeId
    return {0, 0, 0}
end
function box2d.destroyShape(shapeId: shapeId, updateBodyMass: boolean)
end
function box2d.shapeIsValid(shapeId: shapeId): boolean
    return false
end
function box2d.shapeGetType(shapeId: shapeId): shapeType
    return 0
end
function box2d.shapeGetBody(shapeId: shapeId): bodyId
    return {0, 0, 0}
end
function box2d.shapeGetWorld(shapeId: shapeId): worldId
    return {0, 0}
end
function box2d.shapeIsSensor(shapeId: shapeId): boolean
    return false
end
function box2d.shapeSetDensity(shapeId: shapeId, density: number, updateBodyMass: boolean) end
function box2d.shapeGetDensity(shapeId: shapeId): number
    return 0
end
function box2d.shapeSetFriction(shapeId: shapeId, friction: number) end
function box2d.shapeGetFriction(shapeId: shapeId): number
    return 0
end
function box2d.shapeSetRestitution(shapeId: shapeId, restitution: number) end
function box2d.shapeGetRestitution(shapeId: shapeId): number
    return 0
end
function box2d.shapeSetUserMaterial(shapeId: shapeId, material: number) end
function box2d.shapeGetUserMaterial(shapeId: shapeId): number
    return 0
end
function box2d.shapeSetSurfaceMaterial(shapeId: shapeId, material: surfaceMaterialDef) end
function box2d.shapeGetSurfaceMaterial(shapeId: shapeId): surfaceMaterial
    return {} :: surfaceMaterial
end
function box2d.shapeGetFilter(shapeId: shapeId): filter
    return {} :: filter
end
function box2d.shapeSetFilter(shapeId: shapeId, filter: filterDef) end
function box2d.shapeEnableSensorEvents(shapeId: shapeId, flag: boolean) end
function box2d.shapeAreSensorEventsEnabled(shapeId: shapeId): boolean
    return false
end
function box2d.shapeEnableContactEvents(shapeId: shapeId, flag: boolean) end
function box2d.shapeAreContactEventsEnabled(shapeId: shapeId): boolean
    return false
end
function box2d.shapeEnableHitEvents(shapeId: shapeId, flag: boolean) end
function box2d.shapeAreHitEventsEnabled(shapeId: shapeId): boolean
    return false
end
function box2d.shapeTestPoint(shapeId: shapeId, pointX: number, pointY: number): boolean
    return false
end
function box2d.shapeGetCircle(shapeId: shapeId): circle
    return {} :: circle
end
function box2d.shapeGetSegment(shapeId: shapeId): segment
    return {} :: segment
end
function box2d.shapeGetCapsule(shapeId: shapeId): capsule
    return {} :: capsule
end
function box2d.shapeGetPolygon(shapeId: shapeId): polygon
    return {} :: polygon
end
function box2d.shapeSetCircle(shapeId: shapeId, circle: circle) end
function box2d.shapeSetCapsule(shapeId: shapeId, capsule: capsule) end
function box2d.shapeSetSegment(shapeId: shapeId, segment: segment) end
function box2d.shapeSetPolygon(shapeId: shapeId, polygon: polygon) end
function box2d.shapeGetContactCapacity(shapeId: shapeId): number
    return 0
end
function box2d.shapeGetSensorCapacity(shapeId: shapeId): number
    return 0
end
function box2d.shapeGetAABB(shapeId: shapeId): (number, number, number, number)
    return 0, 0, 0, 0
end
function box2d.shapeComputeMassData(shapeId: shapeId): massData
    return {} :: massData
end
function box2d.shapeGetClosestPoint(shapeId: shapeId, targetX: number, targetY: number): (number, number)
    return 0, 0
end
function box2d.shapeApplyWind(shapeId: shapeId, windX: number, windY: number, drag: number, lift: number, wake: boolean) end

-- Make

function box2d.makeOffsetRoundedBox(halfWidth: number, halfHeight: number, centerX: number, centerY: number, rotationC: number, rotationS: number, radius: number): polygon
    return {} :: polygon
end
function box2d.makeOffsetBox(halfWidth: number, halfHeight: number, centerX: number, centerY: number, rotationC: number, rotationS: number): polygon
    return {} :: polygon
end
function box2d.makeRoundedBox(halfWidth: number, halfHeight: number, radius: number): polygon
    return {} :: polygon
end
function box2d.makeBox(halfWidth: number, halfHeight: number): polygon
    return {} :: polygon
end
function box2d.makeSquare(halfWidth: number): polygon
    return {} :: polygon
end
function box2d.makeOffsetRoundedPolygon(hull: hull, positionX: number, positionY: number, rotationC: number, rotationS: number, radius: number): polygon
    return {} :: polygon
end
function box2d.makeOffsetPolygon(hull: hull, positionX: number, positionY: number, rotationC: number, rotationS: number): polygon
    return {} :: polygon
end
function box2d.makePolygon(hull: hull, radius: number): polygon
    return {} :: polygon
end

return table.freeze(box2d)