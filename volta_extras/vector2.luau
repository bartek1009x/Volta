-- Luau has a very performant built-in vector class, however it is exclusively 3D.
-- We can, however, treat is a 2D one by ignoring the third Z value of the vector and changing a couple of functions.
-- This "vector2" utility helps with that
-- A full 2D vector implementation could be made, but it would be slower than using the built-in vector type

--!strict
--!native

local vector2 = {}

function vector2.create(x: number, y: number): vector
    return vector.create(x, y, 0)
end

function vector2.cross(vec: vector, vec2: vector): number
    return vector.cross(vec, vec2).z
end

function vector2.angle(vec: vector, vec2: vector, axis: vector?): number
    return vector.angle(vec, vec2, axis or vector.create(0, 0, 1))
end

vector2.magnitude = vector.magnitude
vector2.normalize = vector.normalize
vector2.dot = vector.dot
vector2.floor = vector.floor
vector2.ceil = vector.ceil
vector2.abs = vector.abs
vector2.sign = vector.sign
vector2.clamp = vector.clamp
vector2.max = vector.max
vector2.min = vector.min

return table.freeze(vector2)