local box2d = {}

export type worldId = {number}
export type capacity = {
    staticShapeCount: number,
    dynamicShapeCount: number,
    staticBodyCount: number,
    dynamicBodyCount: number,
    contactCount: number
}
export type worldDef = {
    gravity: {x: number, y: number}?,
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

return box2d