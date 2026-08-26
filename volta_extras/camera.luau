-- Based on the camera implementation for Love2D by Matthias Richter
--[[
Copyright (c) 2010-2015 Matthias Richter

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Except as contained in this notice, the name(s) of the above copyright holders
shall not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
]]--

--!strict
--!native

local cos, sin = math.cos, math.sin

type CameraData = {
    x: number,
    y: number,
    scale: number,
    rot: number,
    smoother: smootherFunction,

    _sx: number,
    _sy: number,
    _sw: number,
    _sh: number
}

type smootherFunction = (number, number, number?) -> (number, number)

local camera = {}

export type Camera = typeof(setmetatable({} :: CameraData, camera))

camera.__index = camera

-- Movement interpolators (for camera locking/windowing)
camera.smooth = {}

function camera.smooth.none(): smootherFunction
	return function(dx: number, dy: number) return dx, dy end
end

function camera.smooth.linear(speed: number): smootherFunction
	assert(type(speed) == "number", "Invalid parameter: speed = "..tostring(speed))
	return function(dx: number, dy: number, s: number?)
		-- normalize direction
		local d = math.sqrt(dx*dx+dy*dy)
		local dts = math.min((s or speed) * volta.getDelta(), d) -- prevent overshooting the goal
		if d > 0 then
			dx,dy = dx/d, dy/d
		end

		return dx*dts, dy*dts
	end
end

function camera.smooth.damped(stiffness: number): smootherFunction
	assert(type(stiffness) == "number", "Invalid parameter: stiffness = "..tostring(stiffness))
	return function(dx: number, dy: number, s: number?)
		local dts = volta.getDelta() * (s or stiffness)
		return dx*dts, dy*dts
	end
end


local function new(x: number?, y: number?, zoom: number?, rot: number?, smoother: smootherFunction?): Camera
    local self = {} :: CameraData

   	local vW, vH = volta.window.getSize()
    self.x = x or vW / 2
    self.y = y or vH / 2
    self.scale = zoom or 1
    self.rot = rot or 0
    self.smoother = smoother or camera.smooth.none()

    return setmetatable(self, camera)
end

function camera.lookAt(self: Camera, x: number, y: number): Camera
	self.x, self.y = x, y
	return self
end

function camera.move(self: Camera, dx: number, dy: number): Camera
    self.x += dx
    self.y += dy
	return self
end

function camera.position(self: Camera): (number, number)
	return self.x, self.y
end

function camera.rotate(self: Camera, phi: number): Camera
	self.rot += phi
	return self
end

function camera.rotateTo(self: Camera, phi: number): Camera
	self.rot = phi
	return self
end

function camera.zoom(self: Camera, mul: number): Camera
	self.scale *= mul
	return self
end

function camera.zoomTo(self: Camera, zoom: number): Camera
	self.scale = zoom
	return self
end

function camera.attach(self: Camera, x: number?, y: number?, w: number?, h: number?, noclip: boolean?)
	x, y = x or 0, y or 0
	local vW, vH = volta.window.getSize()
	local w, h = w or vW, h or vH

	self._sx, self._sy, self._sw, self._sh = volta.graphics.getScissor()
	if not noclip then
		volta.graphics.setScissor(x, y, w, h)
	end

	local cx, cy = x+w/2, y+h/2
	volta.graphics.pushCoord()
	volta.graphics.translateCoord(cx, cy)
	volta.graphics.scaleCoord(self.scale, self.scale)
	volta.graphics.rotateCoord(self.rot)
	volta.graphics.translateCoord(-self.x, -self.y)
end

function camera.detach(self: Camera)
	volta.graphics.popCoord()
	volta.graphics.setScissor(self._sx, self._sy, self._sw, self._sh)
end

function camera:draw(...)
	local x, y, w, h: number
	local noclip: boolean
	local func: () -> ()

	local nargs = select("#", ...)
	if nargs == 1 then
		func = ...
	elseif nargs == 5 then
		x, y, w, h, func = ...
	elseif nargs == 6 then
		x, y, w, h, noclip, func = ...
	else
		error("Invalid arguments to camera:draw()")
	end

	self:attach(x, y, w, h, noclip)
	func()
	self:detach()
end

-- world coordinates to camera coordinates
function camera.cameraCoords(self: Camera, x: number, y: number, ox: number?, oy: number?, w: number?, h: number?): (number, number)
	ox, oy = ox or 0, oy or 0
	local vW, vH = volta.window.getSize()
	local w, h = w or vW, h or vH

	-- x, y = ((x, y) - (self.x, self.y)):rotated(self.rot) * self.scale + center
	local c, s = cos(self.rot), sin(self.rot)
	x -= self.x
	y -= self.y
	x, y = c*x - s*y, s*x + c*y
	return x*self.scale + w/2 + ox, y*self.scale + h/2 + oy
end

-- camera coordinates to world coordinates
function camera.worldCoords(self: Camera, x: number, y: number, ox: number?, oy: number?, w: number?, h: number?): (number, number)
	ox, oy = ox or 0, oy or 0
	local vW, vH = volta.window.getSize()
	local w, h = w or vW, h or vH

	-- x, y = (((x, y) - center) / self.scale):rotated(-self.rot) + (self.x, self.y)
	local c, s = cos(-self.rot), sin(-self.rot)
	x, y = (x - w/2 - ox) / self.scale, (y - h/2 - oy) / self.scale
	x, y = c*x - s*y, s*x + c*y
	return x+self.x, y+self.y
end

function camera.mousePosition(self: Camera, ox: number?, oy: number?, w: number?, h: number?): (number, number)
	local mx, my = volta.input.getMousePosition()
	return self:worldCoords(mx, my, ox, oy, w, h)
end

-- camera scrolling utilities
function camera.lockX(self: Camera, x: number, smoother: smootherFunction?, ...): Camera
	local dx, _ = (smoother or self.smoother) (x - self.x, self.y, ...)
	self.x += dx
	return self
end

function camera.lockY(self: Camera, y: number, smoother: smootherFunction?, ...): Camera
	local _, dy = (smoother or self.smoother)(self.x, y - self.y, ...)
	self.y += dy
	return self
end

function camera.lockPosition(self: Camera, x: number, y: number, smoother: smootherFunction?, ...): Camera
	return self:move((smoother or self.smoother)(x - self.x, y - self.y, ...))
end

function camera.lockWindow(self: Camera, x: number, y: number, x_min: number, x_max: number, y_min: number, y_max: number, smoother: smootherFunction?, ...)
	-- figure out displacement in camera coordinates
	x, y = self:cameraCoords(x, y)
	local dx, dy = 0,0
	if x < x_min then
		dx = x - x_min
	elseif x > x_max then
		dx = x - x_max
	end
	if y < y_min then
		dy = y - y_min
	elseif y > y_max then
		dy = y - y_max
	end

	-- transform displacement to movement in world coordinates
	local c, s = cos(-self.rot), sin(-self.rot)
	dx, dy = (c*dx - s*dy) / self.scale, (s*dx + c*dy) / self.scale

	-- move
	self:move((smoother or self.smoother)(dx, dy, ...))
end

-- the module
return table.freeze(setmetatable({new = new, smooth = camera.smooth},
	{__call = function(_, ...) return new(...) end}))