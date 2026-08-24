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
export type overlapResultCallback = (shapeId: shapeId) -> boolean

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

export type queryFilter = {
    categoryBits: number,
    maskBits: number,
}
export type treeStats = {
    nodeVisits: number,
    leafVisits: number,
}
export type rayResult = {
    hit: boolean,
    nodeVisits: number,
    leafVisits: number,
    shapeId: shapeId?,
    point: vec2?,
    normal: vec2?,
    fraction: number?,
}
export type castResultCallback = (shapeId, number, number, number, number, number) -> number
export type shapeProxy = {
    points: {vec2},
    count: number,
    radius: number,
}
export type plane = {
    normal: vec2,
    offset: number,
}
export type planeResult = {
    plane: plane,
    point: vec2,
    hit: boolean,
}
export type planeResultCallback = (shapeId: shapeId, result: planeResult) -> boolean

export type jointId = {number}
export type jointType = number
export type jointEvent = {
    jointId: jointId,
}
export type jointEvents = {
    jointEvents: {jointEvent},
    count: number,
}
export type jointDef = {
    bodyIdA: bodyId?,
    bodyIdB: bodyId?,
    localFrameA: transform?,
    localFrameB: transform?,
    forceThreshold: number?,
    torqueThreshold: number?,
    constraintHertz: number?,
    constraintDampingRatio: number?,
    drawScale: number?,
    collideConnected: boolean?,
}
export type distanceJointDef = {
    base: jointDef?,
    length: number?,
    enableSpring: boolean?,
    lowerSpringForce: number?,
    upperSpringForce: number?,
    hertz: number?,
    dampingRatio: number?,
    enableLimit: boolean?,
    minLength: number?,
    maxLength: number?,
    enableMotor: boolean?,
    maxMotorForce: number?,
    motorSpeed: number?,
}
export type filterJointDef = {
    base: jointDef?,
}
export type motorJointDef = {
    base: jointDef?,
    linearVelocity: vec2?,
    maxVelocityForce: number?,
    angularVelocity: number?,
    maxVelocityTorque: number?,
    linearHertz: number?,
    linearDampingRatio: number?,
    maxSpringForce: number?,
    angularHertz: number?,
    angularDampingRatio: number?,
    maxSpringTorque: number?,
}
export type moverJointDef = {
    base: jointDef?,
    linearVelocity: vec2?,
    maxVelocityForce: vec2?,
}
export type pogoJointDef = {
    base: jointDef?,
    normal: vec2?,
    hertz: number?,
    dampingRatio: number?,
    restLength: number?,
    maxTensionForce: number?,
    maxCompressionForce: number?,
    impulse: number?,
    velocity: number?,
}
export type prismaticJointDef = {
    base: jointDef?,
    enableSpring: boolean?,
    hertz: number?,
    dampingRatio: number?,
    targetTranslation: number?,
    enableLimit: boolean?,
    lowerTranslation: number?,
    upperTranslation: number?,
    enableMotor: boolean?,
    maxMotorForce: number?,
    motorSpeed: number?,
}
export type revoluteJointDef = {
    base: jointDef?,
    targetAngle: number?,
    enableSpring: boolean?,
    hertz: number?,
    dampingRatio: number?,
    enableLimit: boolean?,
    lowerAngle: number?,
    upperAngle: number?,
    enableMotor: boolean?,
    maxMotorTorque: number?,
    motorSpeed: number?,
}
export type weldJointDef = {
    base: jointDef?,
    linearHertz: number?,
    angularHertz: number?,
    linearDampingRatio: number?,
    angularDampingRatio: number?,
}
export type wheelJointDef = {
    base: jointDef?,
    enableSpring: boolean?,
    hertz: number?,
    dampingRatio: number?,
    enableLimit: boolean?,
    lowerTranslation: number?,
    upperTranslation: number?,
    enableMotor: boolean?,
    maxMotorTorque: number?,
    motorSpeed: number?,
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
function box2d.worldOverlapAABB(worldId: worldId, originX: number, originY: number, lowerX: number, lowerY: number, upperX: number, upperY: number, filter: queryFilter, callback: overlapResultCallback): treeStats
    return {} :: treeStats
end
function box2d.worldOverlapShape(worldId: worldId, originX: number, originY: number, proxy: shapeProxy, filter: queryFilter, callback: overlapResultCallback): treeStats
    return {} :: treeStats
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

-- Raycast

function box2d.worldCastRay(worldId: worldId, originX: number, originY: number, translationX: number, translationY: number, filter: queryFilter, callback: castResultCallback): treeStats
    return {} :: treeStats
end
function box2d.worldCastRayClosest(worldId: worldId, originX: number, originY: number, translationX: number, translationY: number, filter: queryFilter): rayResult
    return {} :: rayResult
end
function box2d.worldCastShape(worldId: worldId, originX: number, originY: number, proxy: shapeProxy, translationX: number, translationY: number, filter: queryFilter, callback: castResultCallback): treeStats
    return {} :: treeStats
end
function box2d.worldCastMover(worldId: worldId, originX: number, originY: number, mover: capsule, translationX: number, translationY: number, filter: queryFilter): number
    return 0
end
function box2d.worldCollideMover(worldId: worldId, originX: number, originY: number, mover: capsule, filter: queryFilter, callback: planeResultCallback) end

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
function box2d.bodyGetShapes(bodyId: bodyId): {shapeId}
    return {}
end
function box2d.bodyGetJointCount(bodyId: bodyId): number
    return 0
end
function box2d.bodyGetJoints(bodyId: bodyId): {jointId}
    return {}
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

-- Joints

function box2d.worldGetJointEvents(worldId: worldId): jointEvents
    return {} :: jointEvents
end
function box2d.destroyJoint(jointId: jointId) end
function box2d.jointIsValid(jointId: jointId): boolean
    return false
end
function box2d.jointGetType(jointId: jointId): jointType
    return 0
end
function box2d.jointGetBodyA(jointId: jointId): bodyId
    return {0, 0, 0}
end
function box2d.jointGetBodyB(jointId: jointId): bodyId
    return {0, 0, 0}
end
function box2d.jointGetWorld(jointId: jointId): worldId
    return {0, 0}
end
function box2d.jointSetLocalFrameA(jointId: jointId, localFrame: transform) end
function box2d.jointGetLocalFrameA(jointId: jointId): transform
    return {} :: transform
end
function box2d.jointSetLocalFrameB(jointId: jointId, localFrame: transform) end
function box2d.jointGetLocalFrameB(jointId: jointId): transform
    return {} :: transform
end
function box2d.jointSetCollideConnected(jointId: jointId, shouldCollide: boolean) end
function box2d.jointGetCollideConnected(jointId: jointId): boolean
    return false
end
function box2d.jointWakeBodies(jointId: jointId) end
function box2d.jointGetConstraintForce(jointId: jointId): (number, number)
    return 0, 0
end
function box2d.jointGetConstraintTorque(jointId: jointId): number
    return 0
end
function box2d.jointGetLinearSeparation(jointId: jointId): number
    return 0
end
function box2d.jointGetAngularSeparation(jointId: jointId): number
    return 0
end
function box2d.jointSetConstraintTuning(jointId: jointId, hertz: number, dampingRatio: number) end
function box2d.jointGetConstraintTuning(jointId: jointId): (number, number)
    return 0, 0
end
function box2d.jointSetForceThreshold(jointId: jointId, threshold: number) end
function box2d.jointGetForceThreshold(jointId: jointId): number
    return 0
end
function box2d.jointSetTorqueThreshold(jointId: jointId, threshold: number) end
function box2d.jointGetTorqueThreshold(jointId: jointId): number
    return 0
end

-- Distance joint

function box2d.createDistanceJoint(worldId: worldId, def: distanceJointDef): jointId
    return {0, 0, 0}
end
function box2d.distanceJointSetLength(jointId: jointId, length: number) end
function box2d.distanceJointGetLength(jointId: jointId): number
    return 0
end
function box2d.distanceJointEnableSpring(jointId: jointId, enableSpring: boolean) end
function box2d.distanceJointIsSpringEnabled(jointId: jointId): boolean
    return false
end
function box2d.distanceJointSetSpringForceRange(jointId: jointId, lowerForce: number, upperForce: number) end
function box2d.distanceJointGetSpringForceRange(jointId: jointId): (number, number)
    return 0, 0
end
function box2d.distanceJointSetSpringHertz(jointId: jointId, hertz: number) end
function box2d.distanceJointSetSpringDampingRatio(jointId: jointId, dampingRatio: number) end
function box2d.distanceJointGetSpringHertz(jointId: jointId): number
    return 0
end
function box2d.distanceJointGetSpringDampingRatio(jointId: jointId): number
    return 0
end
function box2d.distanceJointEnableLimit(jointId: jointId, enableLimit: boolean) end
function box2d.distanceJointIsLimitEnabled(jointId: jointId): boolean
    return false
end
function box2d.distanceJointSetLengthRange(jointId: jointId, minLength: number, maxLength: number) end
function box2d.distanceJointGetMinLength(jointId: jointId): number
    return 0
end
function box2d.distanceJointGetMaxLength(jointId: jointId): number
    return 0
end
function box2d.distanceJointGetCurrentLength(jointId: jointId): number
    return 0
end
function box2d.distanceJointEnableMotor(jointId: jointId, enableMotor: boolean) end
function box2d.distanceJointIsMotorEnabled(jointId: jointId): boolean
    return false
end
function box2d.distanceJointSetMotorSpeed(jointId: jointId, motorSpeed: number) end
function box2d.distanceJointGetMotorSpeed(jointId: jointId): number
    return 0
end
function box2d.distanceJointSetMaxMotorForce(jointId: jointId, force: number) end
function box2d.distanceJointGetMaxMotorForce(jointId: jointId): number
    return 0
end
function box2d.distanceJointGetMotorForce(jointId: jointId): number
    return 0
end

-- Filter joint

function box2d.createFilterJoint(worldId: worldId, def: filterJointDef): jointId
    return {0, 0, 0}
end

-- Motor joint

function box2d.createMotorJoint(worldId: worldId, def: motorJointDef): jointId
    return {0, 0, 0}
end
function box2d.motorJointSetLinearVelocity(jointId: jointId, velocityX: number, velocityY: number) end
function box2d.motorJointGetLinearVelocity(jointId: jointId): (number, number)
    return 0, 0
end
function box2d.motorJointSetAngularVelocity(jointId: jointId, velocity: number) end
function box2d.motorJointGetAngularVelocity(jointId: jointId): number
    return 0
end
function box2d.motorJointSetMaxVelocityForce(jointId: jointId, maxForce: number) end
function box2d.motorJointGetMaxVelocityForce(jointId: jointId): number
    return 0
end
function box2d.motorJointSetMaxVelocityTorque(jointId: jointId, maxTorque: number) end
function box2d.motorJointGetMaxVelocityTorque(jointId: jointId): number
    return 0
end
function box2d.motorJointSetLinearHertz(jointId: jointId, hertz: number) end
function box2d.motorJointGetLinearHertz(jointId: jointId): number
    return 0
end
function box2d.motorJointSetLinearDampingRatio(jointId: jointId, damping: number) end
function box2d.motorJointGetLinearDampingRatio(jointId: jointId): number
    return 0
end
function box2d.motorJointSetAngularHertz(jointId: jointId, hertz: number) end
function box2d.motorJointGetAngularHertz(jointId: jointId): number
    return 0
end
function box2d.motorJointSetAngularDampingRatio(jointId: jointId, damping: number) end
function box2d.motorJointGetAngularDampingRatio(jointId: jointId): number
    return 0
end
function box2d.motorJointSetMaxSpringForce(jointId: jointId, maxForce: number) end
function box2d.motorJointGetMaxSpringForce(jointId: jointId): number
    return 0
end
function box2d.motorJointSetMaxSpringTorque(jointId: jointId, maxTorque: number) end
function box2d.motorJointGetMaxSpringTorque(jointId: jointId): number
    return 0
end

-- Mover joint

function box2d.createMoverJoint(worldId: worldId, def: moverJointDef): jointId
    return {0, 0, 0}
end
function box2d.moverJointSetLinearVelocity(jointId: jointId, velocityX: number, velocityY: number) end
function box2d.moverJointGetLinearVelocity(jointId: jointId): (number, number)
    return 0, 0
end
function box2d.moverJointSetMaxVelocityForce(jointId: jointId, maxForceX: number, maxForceY: number) end
function box2d.moverJointGetMaxVelocityForce(jointId: jointId): (number, number)
    return 0, 0
end

-- Pogo joint

function box2d.createPogoJoint(worldId: worldId, def: pogoJointDef): jointId
    return {0, 0, 0}
end
function box2d.pogoJointSetRestLength(jointId: jointId, length: number) end
function box2d.pogoJointGetRestLength(jointId: jointId): number
    return 0
end
function box2d.pogoJointSetSpringHertz(jointId: jointId, hertz: number) end
function box2d.pogoJointGetSpringHertz(jointId: jointId): number
    return 0
end
function box2d.pogoJointSetSpringDampingRatio(jointId: jointId, dampingRatio: number) end
function box2d.pogoJointGetSpringDampingRatio(jointId: jointId): number
    return 0
end
function box2d.pogoJointGetLength(jointId: jointId): number
    return 0
end
function box2d.pogoJointGetVelocity(jointId: jointId): number
    return 0
end
function box2d.pogoJointGetImpulse(jointId: jointId): number
    return 0
end

-- Prismatic joint

function box2d.createPrismaticJoint(worldId: worldId, def: prismaticJointDef): jointId
    return {0, 0, 0}
end
function box2d.prismaticJointEnableSpring(jointId: jointId, enableSpring: boolean) end
function box2d.prismaticJointIsSpringEnabled(jointId: jointId): boolean
    return false
end
function box2d.prismaticJointSetSpringHertz(jointId: jointId, hertz: number) end
function box2d.prismaticJointGetSpringHertz(jointId: jointId): number
    return 0
end
function box2d.prismaticJointSetSpringDampingRatio(jointId: jointId, dampingRatio: number) end
function box2d.prismaticJointGetSpringDampingRatio(jointId: jointId): number
    return 0
end
function box2d.prismaticJointSetTargetTranslation(jointId: jointId, translation: number) end
function box2d.prismaticJointGetTargetTranslation(jointId: jointId): number
    return 0
end
function box2d.prismaticJointEnableLimit(jointId: jointId, enableLimit: boolean) end
function box2d.prismaticJointIsLimitEnabled(jointId: jointId): boolean
    return false
end
function box2d.prismaticJointGetLowerLimit(jointId: jointId): number
    return 0
end
function box2d.prismaticJointGetUpperLimit(jointId: jointId): number
    return 0
end
function box2d.prismaticJointSetLimits(jointId: jointId, lower: number, upper: number) end
function box2d.prismaticJointEnableMotor(jointId: jointId, enableMotor: boolean) end
function box2d.prismaticJointIsMotorEnabled(jointId: jointId): boolean
    return false
end
function box2d.prismaticJointSetMotorSpeed(jointId: jointId, motorSpeed: number) end
function box2d.prismaticJointGetMotorSpeed(jointId: jointId): number
    return 0
end
function box2d.prismaticJointSetMaxMotorForce(jointId: jointId, force: number) end
function box2d.prismaticJointGetMaxMotorForce(jointId: jointId): number
    return 0
end
function box2d.prismaticJointGetMotorForce(jointId: jointId): number
    return 0
end
function box2d.prismaticJointGetTranslation(jointId: jointId): number
    return 0
end
function box2d.prismaticJointGetSpeed(jointId: jointId): number
    return 0
end

-- Revolute joint

function box2d.createRevoluteJoint(worldId: worldId, def: revoluteJointDef): jointId
    return {0, 0, 0}
end
function box2d.revoluteJointEnableSpring(jointId: jointId, enableSpring: boolean) end
function box2d.revoluteJointIsSpringEnabled(jointId: jointId): boolean
    return false
end
function box2d.revoluteJointSetSpringHertz(jointId: jointId, hertz: number) end
function box2d.revoluteJointGetSpringHertz(jointId: jointId): number
    return 0
end
function box2d.revoluteJointSetSpringDampingRatio(jointId: jointId, dampingRatio: number) end
function box2d.revoluteJointGetSpringDampingRatio(jointId: jointId): number
    return 0
end
function box2d.revoluteJointSetTargetAngle(jointId: jointId, angle: number) end
function box2d.revoluteJointGetTargetAngle(jointId: jointId): number
    return 0
end
function box2d.revoluteJointGetAngle(jointId: jointId): number
    return 0
end
function box2d.revoluteJointEnableLimit(jointId: jointId, enableLimit: boolean) end
function box2d.revoluteJointIsLimitEnabled(jointId: jointId): boolean
    return false
end
function box2d.revoluteJointGetLowerLimit(jointId: jointId): number
    return 0
end
function box2d.revoluteJointGetUpperLimit(jointId: jointId): number
    return 0
end
function box2d.revoluteJointSetLimits(jointId: jointId, lower: number, upper: number) end
function box2d.revoluteJointEnableMotor(jointId: jointId, enableMotor: boolean) end
function box2d.revoluteJointIsMotorEnabled(jointId: jointId): boolean
    return false
end
function box2d.revoluteJointSetMotorSpeed(jointId: jointId, motorSpeed: number) end
function box2d.revoluteJointGetMotorSpeed(jointId: jointId): number
    return 0
end
function box2d.revoluteJointGetMotorTorque(jointId: jointId): number
    return 0
end
function box2d.revoluteJointSetMaxMotorTorque(jointId: jointId, torque: number) end
function box2d.revoluteJointGetMaxMotorTorque(jointId: jointId): number
    return 0
end

-- Weld joint

function box2d.createWeldJoint(worldId: worldId, def: weldJointDef): jointId
    return {0, 0, 0}
end
function box2d.weldJointSetLinearHertz(jointId: jointId, hertz: number) end
function box2d.weldJointGetLinearHertz(jointId: jointId): number
    return 0
end
function box2d.weldJointSetLinearDampingRatio(jointId: jointId, dampingRatio: number) end
function box2d.weldJointGetLinearDampingRatio(jointId: jointId): number
    return 0
end
function box2d.weldJointSetAngularHertz(jointId: jointId, hertz: number) end
function box2d.weldJointGetAngularHertz(jointId: jointId): number
    return 0
end
function box2d.weldJointSetAngularDampingRatio(jointId: jointId, dampingRatio: number) end
function box2d.weldJointGetAngularDampingRatio(jointId: jointId): number
    return 0
end

-- Wheel joint

function box2d.createWheelJoint(worldId: worldId, def: wheelJointDef): jointId
    return {0, 0, 0}
end
function box2d.wheelJointEnableSpring(jointId: jointId, enableSpring: boolean) end
function box2d.wheelJointIsSpringEnabled(jointId: jointId): boolean
    return false
end
function box2d.wheelJointSetSpringHertz(jointId: jointId, hertz: number) end
function box2d.wheelJointGetSpringHertz(jointId: jointId): number
    return 0
end
function box2d.wheelJointSetSpringDampingRatio(jointId: jointId, dampingRatio: number) end
function box2d.wheelJointGetSpringDampingRatio(jointId: jointId): number
    return 0
end
function box2d.wheelJointEnableLimit(jointId: jointId, enableLimit: boolean) end
function box2d.wheelJointIsLimitEnabled(jointId: jointId): boolean
    return false
end
function box2d.wheelJointGetLowerLimit(jointId: jointId): number
    return 0
end
function box2d.wheelJointGetUpperLimit(jointId: jointId): number
    return 0
end
function box2d.wheelJointSetLimits(jointId: jointId, lower: number, upper: number) end
function box2d.wheelJointEnableMotor(jointId: jointId, enableMotor: boolean) end
function box2d.wheelJointIsMotorEnabled(jointId: jointId): boolean
    return false
end
function box2d.wheelJointSetMotorSpeed(jointId: jointId, motorSpeed: number) end
function box2d.wheelJointGetMotorSpeed(jointId: jointId): number
    return 0
end
function box2d.wheelJointSetMaxMotorTorque(jointId: jointId, torque: number) end
function box2d.wheelJointGetMaxMotorTorque(jointId: jointId): number
    return 0
end
function box2d.wheelJointGetMotorTorque(jointId: jointId): number
    return 0
end

return table.freeze(box2d)