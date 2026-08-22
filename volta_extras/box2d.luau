--!strict
--
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

return box2d