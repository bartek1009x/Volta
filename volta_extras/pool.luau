--!strict
--!native

type PoolData<T> = {
    objects: {T},
    newObject: () -> T
}

local pool = {}

export type Pool<T> = typeof(setmetatable({} :: PoolData<T>, pool))

pool.__index = pool
pool.__len = function<T>(self: Pool<T>)
	return #self.objects
end

function pool.new<T>(newObject: () -> T, initialSize: number?): Pool<T>
    local self = {} :: PoolData<T>
    if (initialSize) then
        self.objects = table.create(initialSize) :: {T}
    else
        self.objects = {} :: {T}
    end
    self.newObject = newObject :: () -> T

    return setmetatable(self, pool)
end

function pool.get<T>(self: Pool<T>): T
    local index: number = #self.objects
    if (index > 0) then
        local obj: T = self.objects[index]
        self.objects[index] = nil
        return obj
    end

    return self.newObject()
end

function pool.free<T>(self: Pool<T>, obj: T)
    self.objects[#self.objects + 1] = obj
end

return table.freeze(pool)