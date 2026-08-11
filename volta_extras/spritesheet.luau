--!strict
--!native

type SpritesheetData = {
    textureId: number,
    regions: {number: {number}}
}

local spritesheet = {}

export type Spritesheet = typeof(setmetatable({} :: SpritesheetData, spritesheet))

spritesheet.__index = spritesheet
spritesheet.__len = function(self: Spritesheet)
	return #self.regions
end

function spritesheet.new(textureId: number, regions: {number: {number}}): Spritesheet
    local self = {} :: SpritesheetData
    self.textureId = textureId
    self.regions = regions
    table.freeze(self.regions)

    return setmetatable(self, spritesheet)
end

function spritesheet.getRegionDimensions(self: Spritesheet, regionIndex: number): {number}
    return self.regions[regionIndex]
end

return table.freeze(spritesheet)