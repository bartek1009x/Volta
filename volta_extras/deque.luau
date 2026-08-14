-- Lute's deque implementation with slight changes.
-- Licensed under MIT
--[[

MIT License

Copyright (c) 2024–2025 Roblox Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

]]

--!strict
--!native

type DequeData<T> = {
	_head: number,
	_tail: number,
	_values: { [number]: T },
}

local deque = {}

export type Deque<T> = typeof(setmetatable({} :: DequeData<T>, deque))

deque.__index = deque
deque.__len = function<T>(self: Deque<T>)
	return self._tail - self._head + 1
end

local dequeLib = {}

function dequeLib.new<T>(initValue: T?): Deque<T>
	local self = {} :: DequeData<T>
	self._values = {}
	self._head, self._tail = 0, if initValue then 0 else -1
	if initValue then
		self._values[self._head] = initValue
	end

	return setmetatable(self, deque)
end

function deque.pushfront<T>(self: Deque<T>, value: T)
	self._head -= 1
	self._values[self._head] = value
end

function deque.pushback<T>(self: Deque<T>, value: T)
	self._tail += 1
	self._values[self._tail] = value
end

function deque.popfront<T>(self: Deque<T>): T
	if self._tail < self._head then
		error("Popping from empty deque")
	end
	local toReturn = self._values[self._head]
	self._values[self._head] = nil
	self._head += 1
	return toReturn
end

function deque.popback<T>(self: Deque<T>): T
	if self._tail < self._head then
		error("Popping from empty deque")
	end
	local toReturn = self._values[self._tail]
	self._values[self._tail] = nil
	self._tail -= 1
	return toReturn
end

function deque.peekfront<T>(self: Deque<T>)
	return self._values[self._head]
end

function deque.peekback<T>(self: Deque<T>)
	return self._values[self._tail]
end

return table.freeze(dequeLib)