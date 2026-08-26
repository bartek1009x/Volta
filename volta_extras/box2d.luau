--!strict

local box2d = {}

export type vec2 = {
    x: number,
    y: number
}
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
export type overlapResultCallback = (shapeId: number) -> boolean

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
export type chainSegment = {
    ghost1: vec2,
    segment: segment,
    ghost2: vec2,
}

export type chainDef = {
    points: {vec2},
    materials: {surfaceMaterialDef}?,
    filter: filterDef?,
    isLoop: boolean?,
    enableSensorEvents: boolean?,
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
    x: number,
    y: number,
    c: number,
    s: number,
    bodyId: number,
    fellAsleep: boolean,
}
export type bodyEvents = {bodyMoveEvent}
export type contactBeginTouchEvent = {
    shapeIdA: number,
    shapeIdB: number,
    contactIdIndex1: number,
    contactIdWorld0: number,
    contactIdGeneration: number,
}
export type contactEndTouchEvent = {
    shapeIdA: number,
    shapeIdB: number,
    contactIdIndex1: number,
    contactIdWorld0: number,
    contactIdGeneration: number,
}
export type contactHitEvent = {
    shapeIdA: number,
    shapeIdB: number,
    contactIdIndex1: number,
    contactIdWorld0: number,
    contactIdGeneration: number,
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
    sensorShapeId: number,
    visitorShapeId: number,
}
export type sensorEndTouchEvent = {
    sensorShapeId: number,
    visitorShapeId: number,
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
    shapeId: number?,
    point: vec2?,
    normal: vec2?,
    fraction: number?,
}
export type castResultCallback = (shapeId: number, pointX: number, pointY: number, normalX: number, normalY: number, fraction: number) -> number
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
export type planeResultCallback = (shapeId: number, result: planeResult) -> boolean

export type jointType = number
export type jointEvent = {
    jointId: number,
}
export type jointEvents = {
    jointEvents: {jointEvent},
    count: number,
}
export type jointDef = {
    bodyIdA: number?,
    bodyIdB: number?,
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
export type explosionDef = {
    maskBits: number?,
    position: vec2?,
    radius: number?,
    falloff: number?,
    impulsePerLength: number?,
}

-- World

function box2d.createWorld(worldDef: worldDef) : number
    return 0
end
function box2d.destroyWorld(worldId: number) end
function box2d.worldIsValid(worldId: number) : boolean
    return false
end
function box2d.worldStep(worldId: number, timeStep: number, subStepCount: number) end
function box2d.worldGetBounds(worldId: number) : (number, number, number, number)
    return 0, 0, 0, 0
end
function box2d.worldGetBodyEvents(worldId: number): bodyEvents
    return {} :: bodyEvents
end
function box2d.worldGetContactEvents(worldId: number): contactEvents
    return {} :: contactEvents
end
function box2d.worldGetSensorEvents(worldId: number): sensorEvents
    return {} :: sensorEvents
end
function box2d.worldOverlapAABB(worldId: number, originX: number, originY: number, lowerX: number, lowerY: number, upperX: number, upperY: number, filter: queryFilter, callback: overlapResultCallback): treeStats
    return {} :: treeStats
end
function box2d.worldOverlapShape(worldId: number, originX: number, originY: number, proxy: shapeProxy, filter: queryFilter, callback: overlapResultCallback): treeStats
    return {} :: treeStats
end
function box2d.worldEnableSleeping(worldId: number, flag: boolean) end
function box2d.worldIsSleepingEnabled(worldId: number) : boolean
    return false
end
function box2d.worldEnableContinuous(worldId: number, flag: boolean) end
function box2d.worldIsContinuousEnabled(worldId: number) : boolean
    return false
end
function box2d.worldSetRestitutionThreshold(worldId: number, value: number) end
function box2d.worldGetRestitutionThreshold(worldId: number) : number
    return 0
end
function box2d.worldSetHitEventThreshold(worldId: number, value: number) end
function box2d.worldGetHitEventThreshold(worldId: number) : number
    return 0
end
function box2d.worldSetGravity(worldId: number, gravityX: number, gravityY: number) end
function box2d.worldGetGravity(worldId: number) : (number, number)
    return 0, 0
end
function box2d.worldExplode(worldId: number, def: explosionDef) end
function box2d.worldSetContactTuning(worldId: number, hertz: number, dampingRatio: number, pushSpeed: number) end
function box2d.worldSetContactRecycleDistance(worldId: number, recycleDistance: number) end
function box2d.worldGetContactRecycleDistance(worldId: number) : number
    return 0
end
function box2d.worldSetMaximumLinearSpeed(worldId: number, maximumLinearSpeed: number) end
function box2d.worldGetMaximumLinearSpeed(worldId: number) : number
    return 0
end
function box2d.worldEnableWarmStarting(worldId: number, flag: boolean) end
function box2d.worldIsWarmStartingEnabled(worldId: number) : boolean
    return false
end
function box2d.worldGetAwakeBodyCount(worldId: number) : number
    return 0
end
function box2d.worldGetCounters(worldId: number) : counters
    return {} :: counters
end
function box2d.worldGetMaxCapacity(worldId: number) : capacity
    return {} :: capacity
end
function box2d.worldSetWorkerCount(worldId: number, count: number) end
function box2d.worldGetWorkerCount(worldId: number) : number
    return 0
end
function box2d.worldGetStateHash(worldId: number) : number
    return 0
end
function box2d.contactIsValid(contactIdIndex1: number, contactIdWorld0: number, contactIdGeneration: number): boolean
    return false
end

-- Raycast

function box2d.worldCastRay(worldId: number, originX: number, originY: number, translationX: number, translationY: number, filter: queryFilter, callback: castResultCallback): treeStats
    return {} :: treeStats
end
function box2d.worldCastRayClosest(worldId: number, originX: number, originY: number, translationX: number, translationY: number, filter: queryFilter): rayResult
    return {} :: rayResult
end
function box2d.worldCastShape(worldId: number, originX: number, originY: number, proxy: shapeProxy, translationX: number, translationY: number, filter: queryFilter, callback: castResultCallback): treeStats
    return {} :: treeStats
end
function box2d.worldCastMover(worldId: number, originX: number, originY: number, mover: capsule, translationX: number, translationY: number, filter: queryFilter): number
    return 0
end
function box2d.worldCollideMover(worldId: number, originX: number, originY: number, mover: capsule, filter: queryFilter, callback: planeResultCallback) end

-- Body

function box2d.createBody(worldId: number, bodyDef: bodyDef): number
    return 0
end
function box2d.destroyBody(bodyId: number) end
function box2d.bodyIsValid(bodyId: number): boolean
    return false
end
function box2d.bodyGetType(bodyId: number): bodyType
    return 0
end
function box2d.bodySetType(bodyId: number, bodyType: bodyType) end
function box2d.bodySetName(bodyId: number, name: string) end
function box2d.bodyGetName(bodyId: number): string?
    return nil
end
function box2d.bodyGetPosition(bodyId: number): (number, number)
    return 0, 0
end
function box2d.bodyGetRotation(bodyId: number): (number, number)
    return 0, 0
end
function box2d.bodyGetTransform(bodyId: number): (number, number, number, number)
    return 0, 0, 0, 0
end
function box2d.bodySetTransform(bodyId: number, positionX: number, positionY: number, rotationC: number, rotationS: number) end
function box2d.bodyGetLocalPoint(bodyId: number, worldPointX: number, worldPointY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetWorldPoint(bodyId: number, localPointX: number, localPointY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetLocalVector(bodyId: number, worldVectorX: number, worldVectorY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetWorldVector(bodyId: number, localVectorX: number, localVectorY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetLinearVelocity(bodyId: number): (number, number)
    return 0, 0
end
function box2d.bodyGetAngularVelocity(bodyId: number): number
    return 0
end
function box2d.bodySetLinearVelocity(bodyId: number, velocityX: number, velocityY: number) end
function box2d.bodySetAngularVelocity(bodyId: number, angularVelocity: number) end
function box2d.bodySetTargetTransform(bodyId: number, positionX: number, positionY: number, rotationC: number, rotationS: number, timeStep: number, wake: boolean) end
function box2d.bodyGetLocalPointVelocity(bodyId: number, localPointX: number, localPointY: number): (number, number)
    return 0, 0
end
function box2d.bodyGetWorldPointVelocity(bodyId: number, worldPointX: number, worldPointY: number): (number, number)
    return 0, 0
end
function box2d.bodyApplyForce(bodyId: number, forceX: number, forceY: number, pointX: number, pointY: number, wake: boolean) end
function box2d.bodyApplyForceToCenter(bodyId: number, forceX: number, forceY: number, wake: boolean) end
function box2d.bodyApplyTorque(bodyId: number, torque: number, wake: boolean) end
function box2d.bodyClearForces(bodyId: number) end
function box2d.bodyApplyLinearImpulse(bodyId: number, impulseX: number, impulseY: number, pointX: number, pointY: number, wake: boolean) end
function box2d.bodyApplyLinearImpulseToCenter(bodyId: number, impulseX: number, impulseY: number, wake: boolean) end
function box2d.bodyApplyAngularImpulse(bodyId: number, impulse: number, wake: boolean) end
function box2d.bodyGetMass(bodyId: number): number
    return 0
end
function box2d.bodyGetRotationalInertia(bodyId: number): number
    return 0
end
function box2d.bodyGetLocalCenter(bodyId: number): (number, number)
    return 0, 0
end
function box2d.bodyGetWorldCenter(bodyId: number): (number, number)
    return 0, 0
end
function box2d.bodySetMassData(bodyId: number, massData: massData) end
function box2d.bodyGetMassData(bodyId: number): massData
    return {} :: massData
end
function box2d.bodyApplyMassFromShapes(bodyId: number) end
function box2d.bodySetLinearDamping(bodyId: number, linearDamping: number) end
function box2d.bodyGetLinearDamping(bodyId: number): number
    return 0
end
function box2d.bodySetAngularDamping(bodyId: number, angularDamping: number) end
function box2d.bodyGetAngularDamping(bodyId: number): number
    return 0
end
function box2d.bodySetGravityScale(bodyId: number, gravityScale: number) end
function box2d.bodyGetGravityScale(bodyId: number): number
    return 0
end
function box2d.bodyIsAwake(bodyId: number): boolean
    return false
end
function box2d.bodySetAwake(bodyId: number, awake: boolean) end
function box2d.bodyWakeTouching(bodyId: number) end
function box2d.bodyEnableSleep(bodyId: number, enableSleep: boolean) end
function box2d.bodyIsSleepEnabled(bodyId: number): boolean
    return false
end
function box2d.bodySetSleepThreshold(bodyId: number, sleepThreshold: number) end
function box2d.bodyGetSleepThreshold(bodyId: number): number
    return 0
end
function box2d.bodySetSafetyFactor(bodyId: number, safetyFactor: number) end
function box2d.bodyGetSafetyFactor(bodyId: number): number
    return 0
end
function box2d.bodyIsEnabled(bodyId: number): boolean
    return false
end
function box2d.bodyDisable(bodyId: number) end
function box2d.bodyEnable(bodyId: number) end
function box2d.bodySetMotionLocks(bodyId: number, locks: motionLocks) end
function box2d.bodyGetMotionLocks(bodyId: number): motionLocks
    return {} :: motionLocks
end
function box2d.bodySetBullet(bodyId: number, flag: boolean) end
function box2d.bodyIsBullet(bodyId: number): boolean
    return false
end
function box2d.bodyEnableContactRecycling(bodyId: number, flag: boolean) end
function box2d.bodyIsContactRecyclingEnabled(bodyId: number): boolean
    return false
end
function box2d.bodyEnableContactEvents(bodyId: number, flag: boolean) end
function box2d.bodyEnableHitEvents(bodyId: number, flag: boolean)
end
function box2d.bodyGetWorld(bodyId: number): number
    return 0
end
function box2d.bodyGetShapeCount(bodyId: number): number
    return 0
end
function box2d.bodyGetShapes(bodyId: number): {number}
    return {}
end
function box2d.bodyGetJointCount(bodyId: number): number
    return 0
end
function box2d.bodyGetJoints(bodyId: number): {number}
    return {}
end
function box2d.bodyGetContactCapacity(bodyId: number): number
    return 0
end
function box2d.bodyComputeAABB(bodyId: number): (number, number, number, number)
    return 0, 0, 0, 0
end

-- Shape

function box2d.createCircleShape(bodyId: number, shapeDef: shapeDef, circle: circle): number
    return 0
end
function box2d.createSegmentShape(bodyId: number, shapeDef: shapeDef, segment: segment): number
    return 0
end
function box2d.createCapsuleShape(bodyId: number, shapeDef: shapeDef, capsule: capsule): number
    return 0
end
function box2d.createPolygonShape(bodyId: number, shapeDef: shapeDef, polygon: polygon): number
    return 0
end
function box2d.destroyShape(shapeId: number, updateBodyMass: boolean)
end
function box2d.shapeIsValid(shapeId: number): boolean
    return false
end
function box2d.shapeGetType(shapeId: number): shapeType
    return 0
end
function box2d.shapeGetBody(shapeId: number): number
    return 0
end
function box2d.shapeGetWorld(shapeId: number): number
    return 0
end
function box2d.shapeIsSensor(shapeId: number): boolean
    return false
end
function box2d.shapeSetDensity(shapeId: number, density: number, updateBodyMass: boolean) end
function box2d.shapeGetDensity(shapeId: number): number
    return 0
end
function box2d.shapeSetFriction(shapeId: number, friction: number) end
function box2d.shapeGetFriction(shapeId: number): number
    return 0
end
function box2d.shapeSetRestitution(shapeId: number, restitution: number) end
function box2d.shapeGetRestitution(shapeId: number): number
    return 0
end
function box2d.shapeSetUserMaterial(shapeId: number, material: number) end
function box2d.shapeGetUserMaterial(shapeId: number): number
    return 0
end
function box2d.shapeSetSurfaceMaterial(shapeId: number, material: surfaceMaterialDef) end
function box2d.shapeGetSurfaceMaterial(shapeId: number): surfaceMaterial
    return {} :: surfaceMaterial
end
function box2d.shapeGetFilter(shapeId: number): filter
    return {} :: filter
end
function box2d.shapeSetFilter(shapeId: number, filter: filterDef) end
function box2d.shapeEnableSensorEvents(shapeId: number, flag: boolean) end
function box2d.shapeAreSensorEventsEnabled(shapeId: number): boolean
    return false
end
function box2d.shapeEnableContactEvents(shapeId: number, flag: boolean) end
function box2d.shapeAreContactEventsEnabled(shapeId: number): boolean
    return false
end
function box2d.shapeEnableHitEvents(shapeId: number, flag: boolean) end
function box2d.shapeAreHitEventsEnabled(shapeId: number): boolean
    return false
end
function box2d.shapeTestPoint(shapeId: number, pointX: number, pointY: number): boolean
    return false
end
function box2d.shapeGetCircle(shapeId: number): circle
    return {} :: circle
end
function box2d.shapeGetSegment(shapeId: number): segment
    return {} :: segment
end
function box2d.shapeGetCapsule(shapeId: number): capsule
    return {} :: capsule
end
function box2d.shapeGetPolygon(shapeId: number): polygon
    return {} :: polygon
end
function box2d.shapeSetCircle(shapeId: number, circle: circle) end
function box2d.shapeSetCapsule(shapeId: number, capsule: capsule) end
function box2d.shapeSetSegment(shapeId: number, segment: segment) end
function box2d.shapeSetPolygon(shapeId: number, polygon: polygon) end
function box2d.shapeGetContactCapacity(shapeId: number): number
    return 0
end
function box2d.shapeGetSensorCapacity(shapeId: number): number
    return 0
end
function box2d.shapeGetAABB(shapeId: number): (number, number, number, number)
    return 0, 0, 0, 0
end
function box2d.shapeComputeMassData(shapeId: number): massData
    return {} :: massData
end
function box2d.shapeGetClosestPoint(shapeId: number, targetX: number, targetY: number): (number, number)
    return 0, 0
end
function box2d.shapeApplyWind(shapeId: number, windX: number, windY: number, drag: number, lift: number, wake: boolean) end
function box2d.createChainSegmentShape(bodyId: number, def: shapeDef, chainSegment: chainSegment): number
    return 0
end
function box2d.shapeGetChainSegment(shapeId: number): chainSegment
    return {} :: chainSegment
end
function box2d.shapeSetChainSegment(shapeId: number, chainSegment: chainSegment) end
function box2d.shapeGetParentChain(shapeId: number): number
    return 0
end

-- Chain

function box2d.createChain(bodyId: number, def: chainDef): number
    return 0
end
function box2d.destroyChain(chainId: number) end
function box2d.chainIsValid(chainId: number): boolean
    return false
end
function box2d.chainGetWorld(chainId: number): number
    return 0
end
function box2d.chainGetSegmentCount(chainId: number): number
    return 0
end
function box2d.chainGetSegments(chainId: number): {number}
    return {}
end
function box2d.chainGetSurfaceMaterialCount(chainId: number): number
    return 0
end
function box2d.chainSetSurfaceMaterial(chainId: number, material: surfaceMaterialDef, materialIndex: number) end
function box2d.chainGetSurfaceMaterial(chainId: number, materialIndex: number): surfaceMaterial
    return {} :: surfaceMaterial
end

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
function box2d.computeHull(points: {vec2}): hull
    return {} :: hull
end

-- Joints

function box2d.worldGetJointEvents(worldId: number): jointEvents
    return {} :: jointEvents
end
function box2d.destroyJoint(jointId: number) end
function box2d.jointIsValid(jointId: number): boolean
    return false
end
function box2d.jointGetType(jointId: number): jointType
    return 0
end
function box2d.jointGetBodyA(jointId: number): number
    return 0
end
function box2d.jointGetBodyB(jointId: number): number
    return 0
end
function box2d.jointGetWorld(jointId: number): number
    return 0
end
function box2d.jointSetLocalFrameA(jointId: number, localFrame: transform) end
function box2d.jointGetLocalFrameA(jointId: number): transform
    return {} :: transform
end
function box2d.jointSetLocalFrameB(jointId: number, localFrame: transform) end
function box2d.jointGetLocalFrameB(jointId: number): transform
    return {} :: transform
end
function box2d.jointSetCollideConnected(jointId: number, shouldCollide: boolean) end
function box2d.jointGetCollideConnected(jointId: number): boolean
    return false
end
function box2d.jointWakeBodies(jointId: number) end
function box2d.jointGetConstraintForce(jointId: number): (number, number)
    return 0, 0
end
function box2d.jointGetConstraintTorque(jointId: number): number
    return 0
end
function box2d.jointGetLinearSeparation(jointId: number): number
    return 0
end
function box2d.jointGetAngularSeparation(jointId: number): number
    return 0
end
function box2d.jointSetConstraintTuning(jointId: number, hertz: number, dampingRatio: number) end
function box2d.jointGetConstraintTuning(jointId: number): (number, number)
    return 0, 0
end
function box2d.jointSetForceThreshold(jointId: number, threshold: number) end
function box2d.jointGetForceThreshold(jointId: number): number
    return 0
end
function box2d.jointSetTorqueThreshold(jointId: number, threshold: number) end
function box2d.jointGetTorqueThreshold(jointId: number): number
    return 0
end

-- Distance joint

function box2d.createDistanceJoint(worldId: number, def: distanceJointDef): number
    return 0
end
function box2d.distanceJointSetLength(jointId: number, length: number) end
function box2d.distanceJointGetLength(jointId: number): number
    return 0
end
function box2d.distanceJointEnableSpring(jointId: number, enableSpring: boolean) end
function box2d.distanceJointIsSpringEnabled(jointId: number): boolean
    return false
end
function box2d.distanceJointSetSpringForceRange(jointId: number, lowerForce: number, upperForce: number) end
function box2d.distanceJointGetSpringForceRange(jointId: number): (number, number)
    return 0, 0
end
function box2d.distanceJointSetSpringHertz(jointId: number, hertz: number) end
function box2d.distanceJointSetSpringDampingRatio(jointId: number, dampingRatio: number) end
function box2d.distanceJointGetSpringHertz(jointId: number): number
    return 0
end
function box2d.distanceJointGetSpringDampingRatio(jointId: number): number
    return 0
end
function box2d.distanceJointEnableLimit(jointId: number, enableLimit: boolean) end
function box2d.distanceJointIsLimitEnabled(jointId: number): boolean
    return false
end
function box2d.distanceJointSetLengthRange(jointId: number, minLength: number, maxLength: number) end
function box2d.distanceJointGetMinLength(jointId: number): number
    return 0
end
function box2d.distanceJointGetMaxLength(jointId: number): number
    return 0
end
function box2d.distanceJointGetCurrentLength(jointId: number): number
    return 0
end
function box2d.distanceJointEnableMotor(jointId: number, enableMotor: boolean) end
function box2d.distanceJointIsMotorEnabled(jointId: number): boolean
    return false
end
function box2d.distanceJointSetMotorSpeed(jointId: number, motorSpeed: number) end
function box2d.distanceJointGetMotorSpeed(jointId: number): number
    return 0
end
function box2d.distanceJointSetMaxMotorForce(jointId: number, force: number) end
function box2d.distanceJointGetMaxMotorForce(jointId: number): number
    return 0
end
function box2d.distanceJointGetMotorForce(jointId: number): number
    return 0
end

-- Filter joint

function box2d.createFilterJoint(worldId: number, def: filterJointDef): number
    return 0
end

-- Motor joint

function box2d.createMotorJoint(worldId: number, def: motorJointDef): number
    return 0
end
function box2d.motorJointSetLinearVelocity(jointId: number, velocityX: number, velocityY: number) end
function box2d.motorJointGetLinearVelocity(jointId: number): (number, number)
    return 0, 0
end
function box2d.motorJointSetAngularVelocity(jointId: number, velocity: number) end
function box2d.motorJointGetAngularVelocity(jointId: number): number
    return 0
end
function box2d.motorJointSetMaxVelocityForce(jointId: number, maxForce: number) end
function box2d.motorJointGetMaxVelocityForce(jointId: number): number
    return 0
end
function box2d.motorJointSetMaxVelocityTorque(jointId: number, maxTorque: number) end
function box2d.motorJointGetMaxVelocityTorque(jointId: number): number
    return 0
end
function box2d.motorJointSetLinearHertz(jointId: number, hertz: number) end
function box2d.motorJointGetLinearHertz(jointId: number): number
    return 0
end
function box2d.motorJointSetLinearDampingRatio(jointId: number, damping: number) end
function box2d.motorJointGetLinearDampingRatio(jointId: number): number
    return 0
end
function box2d.motorJointSetAngularHertz(jointId: number, hertz: number) end
function box2d.motorJointGetAngularHertz(jointId: number): number
    return 0
end
function box2d.motorJointSetAngularDampingRatio(jointId: number, damping: number) end
function box2d.motorJointGetAngularDampingRatio(jointId: number): number
    return 0
end
function box2d.motorJointSetMaxSpringForce(jointId: number, maxForce: number) end
function box2d.motorJointGetMaxSpringForce(jointId: number): number
    return 0
end
function box2d.motorJointSetMaxSpringTorque(jointId: number, maxTorque: number) end
function box2d.motorJointGetMaxSpringTorque(jointId: number): number
    return 0
end

-- Mover joint

function box2d.createMoverJoint(worldId: number, def: moverJointDef): number
    return 0
end
function box2d.moverJointSetLinearVelocity(jointId: number, velocityX: number, velocityY: number) end
function box2d.moverJointGetLinearVelocity(jointId: number): (number, number)
    return 0, 0
end
function box2d.moverJointSetMaxVelocityForce(jointId: number, maxForceX: number, maxForceY: number) end
function box2d.moverJointGetMaxVelocityForce(jointId: number): (number, number)
    return 0, 0
end

-- Pogo joint

function box2d.createPogoJoint(worldId: number, def: pogoJointDef): number
    return 0
end
function box2d.pogoJointSetRestLength(jointId: number, length: number) end
function box2d.pogoJointGetRestLength(jointId: number): number
    return 0
end
function box2d.pogoJointSetSpringHertz(jointId: number, hertz: number) end
function box2d.pogoJointGetSpringHertz(jointId: number): number
    return 0
end
function box2d.pogoJointSetSpringDampingRatio(jointId: number, dampingRatio: number) end
function box2d.pogoJointGetSpringDampingRatio(jointId: number): number
    return 0
end
function box2d.pogoJointGetLength(jointId: number): number
    return 0
end
function box2d.pogoJointGetVelocity(jointId: number): number
    return 0
end
function box2d.pogoJointGetImpulse(jointId: number): number
    return 0
end

-- Prismatic joint

function box2d.createPrismaticJoint(worldId: number, def: prismaticJointDef): number
    return 0
end
function box2d.prismaticJointEnableSpring(jointId: number, enableSpring: boolean) end
function box2d.prismaticJointIsSpringEnabled(jointId: number): boolean
    return false
end
function box2d.prismaticJointSetSpringHertz(jointId: number, hertz: number) end
function box2d.prismaticJointGetSpringHertz(jointId: number): number
    return 0
end
function box2d.prismaticJointSetSpringDampingRatio(jointId: number, dampingRatio: number) end
function box2d.prismaticJointGetSpringDampingRatio(jointId: number): number
    return 0
end
function box2d.prismaticJointSetTargetTranslation(jointId: number, translation: number) end
function box2d.prismaticJointGetTargetTranslation(jointId: number): number
    return 0
end
function box2d.prismaticJointEnableLimit(jointId: number, enableLimit: boolean) end
function box2d.prismaticJointIsLimitEnabled(jointId: number): boolean
    return false
end
function box2d.prismaticJointGetLowerLimit(jointId: number): number
    return 0
end
function box2d.prismaticJointGetUpperLimit(jointId: number): number
    return 0
end
function box2d.prismaticJointSetLimits(jointId: number, lower: number, upper: number) end
function box2d.prismaticJointEnableMotor(jointId: number, enableMotor: boolean) end
function box2d.prismaticJointIsMotorEnabled(jointId: number): boolean
    return false
end
function box2d.prismaticJointSetMotorSpeed(jointId: number, motorSpeed: number) end
function box2d.prismaticJointGetMotorSpeed(jointId: number): number
    return 0
end
function box2d.prismaticJointSetMaxMotorForce(jointId: number, force: number) end
function box2d.prismaticJointGetMaxMotorForce(jointId: number): number
    return 0
end
function box2d.prismaticJointGetMotorForce(jointId: number): number
    return 0
end
function box2d.prismaticJointGetTranslation(jointId: number): number
    return 0
end
function box2d.prismaticJointGetSpeed(jointId: number): number
    return 0
end

-- Revolute joint

function box2d.createRevoluteJoint(worldId: number, def: revoluteJointDef): number
    return 0
end
function box2d.revoluteJointEnableSpring(jointId: number, enableSpring: boolean) end
function box2d.revoluteJointIsSpringEnabled(jointId: number): boolean
    return false
end
function box2d.revoluteJointSetSpringHertz(jointId: number, hertz: number) end
function box2d.revoluteJointGetSpringHertz(jointId: number): number
    return 0
end
function box2d.revoluteJointSetSpringDampingRatio(jointId: number, dampingRatio: number) end
function box2d.revoluteJointGetSpringDampingRatio(jointId: number): number
    return 0
end
function box2d.revoluteJointSetTargetAngle(jointId: number, angle: number) end
function box2d.revoluteJointGetTargetAngle(jointId: number): number
    return 0
end
function box2d.revoluteJointGetAngle(jointId: number): number
    return 0
end
function box2d.revoluteJointEnableLimit(jointId: number, enableLimit: boolean) end
function box2d.revoluteJointIsLimitEnabled(jointId: number): boolean
    return false
end
function box2d.revoluteJointGetLowerLimit(jointId: number): number
    return 0
end
function box2d.revoluteJointGetUpperLimit(jointId: number): number
    return 0
end
function box2d.revoluteJointSetLimits(jointId: number, lower: number, upper: number) end
function box2d.revoluteJointEnableMotor(jointId: number, enableMotor: boolean) end
function box2d.revoluteJointIsMotorEnabled(jointId: number): boolean
    return false
end
function box2d.revoluteJointSetMotorSpeed(jointId: number, motorSpeed: number) end
function box2d.revoluteJointGetMotorSpeed(jointId: number): number
    return 0
end
function box2d.revoluteJointGetMotorTorque(jointId: number): number
    return 0
end
function box2d.revoluteJointSetMaxMotorTorque(jointId: number, torque: number) end
function box2d.revoluteJointGetMaxMotorTorque(jointId: number): number
    return 0
end

-- Weld joint

function box2d.createWeldJoint(worldId: number, def: weldJointDef): number
    return 0
end
function box2d.weldJointSetLinearHertz(jointId: number, hertz: number) end
function box2d.weldJointGetLinearHertz(jointId: number): number
    return 0
end
function box2d.weldJointSetLinearDampingRatio(jointId: number, dampingRatio: number) end
function box2d.weldJointGetLinearDampingRatio(jointId: number): number
    return 0
end
function box2d.weldJointSetAngularHertz(jointId: number, hertz: number) end
function box2d.weldJointGetAngularHertz(jointId: number): number
    return 0
end
function box2d.weldJointSetAngularDampingRatio(jointId: number, dampingRatio: number) end
function box2d.weldJointGetAngularDampingRatio(jointId: number): number
    return 0
end

-- Wheel joint

function box2d.createWheelJoint(worldId: number, def: wheelJointDef): number
    return 0
end
function box2d.wheelJointEnableSpring(jointId: number, enableSpring: boolean) end
function box2d.wheelJointIsSpringEnabled(jointId: number): boolean
    return false
end
function box2d.wheelJointSetSpringHertz(jointId: number, hertz: number) end
function box2d.wheelJointGetSpringHertz(jointId: number): number
    return 0
end
function box2d.wheelJointSetSpringDampingRatio(jointId: number, dampingRatio: number) end
function box2d.wheelJointGetSpringDampingRatio(jointId: number): number
    return 0
end
function box2d.wheelJointEnableLimit(jointId: number, enableLimit: boolean) end
function box2d.wheelJointIsLimitEnabled(jointId: number): boolean
    return false
end
function box2d.wheelJointGetLowerLimit(jointId: number): number
    return 0
end
function box2d.wheelJointGetUpperLimit(jointId: number): number
    return 0
end
function box2d.wheelJointSetLimits(jointId: number, lower: number, upper: number) end
function box2d.wheelJointEnableMotor(jointId: number, enableMotor: boolean) end
function box2d.wheelJointIsMotorEnabled(jointId: number): boolean
    return false
end
function box2d.wheelJointSetMotorSpeed(jointId: number, motorSpeed: number) end
function box2d.wheelJointGetMotorSpeed(jointId: number): number
    return 0
end
function box2d.wheelJointSetMaxMotorTorque(jointId: number, torque: number) end
function box2d.wheelJointGetMaxMotorTorque(jointId: number): number
    return 0
end
function box2d.wheelJointGetMotorTorque(jointId: number): number
    return 0
end

return table.freeze(box2d)