-- Type definitions, mainly for the Luau LSP
--!strict

const spritesheet = require("./volta_extras/spritesheet")

local volta = {}

function volta.exit() end
function volta.getPlaytime(): number
    return 0
end
function volta.getDelta(): number
    return 0
end
function volta.getVersion(): string
    return ""
end

volta.window = {}

function volta.window.setTitle(title: string) end
function volta.window.setWindowIcon(iconPath: string) end
function volta.window.setFullscreen(enable: boolean) end
function volta.window.setBorderless(enable: boolean) end
function volta.window.setResizable(enable: boolean) end
function volta.window.setPosition(x: number, y: number) end
function volta.window.setSize(width: number, height: number) end
function volta.window.setMinSize(minWidth: number, minHeight: number) end
function volta.window.setMaxSize(maxWidth: number, maxHeight: number) end
function volta.window.hasFocus(): boolean
    return false
end
function volta.window.getTitle(): string
    return ""
end
function volta.window.isFullscreen(): boolean
    return false
end
function volta.window.isBorderless(): boolean
    return false
end
function volta.window.isResizable(): boolean
    return false
end
function volta.window.getPosition(): number
    return 0, 0
end
function volta.window.getSize(): number
    return 0, 0
end
function volta.window.getMinSize(): number
    return 0, 0
end
function volta.window.getMaxSize(): number
    return 0, 0
end

volta.graphics = {}

function volta.graphics.setCursorVisibility(enable: boolean) end
function volta.graphics.isCursorVisible(): boolean
    return false
end
function volta.graphics.setVsync(enable: boolean) end
function volta.graphics.isVsyncEnabled(): boolean
    return false
end
function volta.graphics.setDrawColor(r: number, g: number, b: number, a: number) end
function volta.graphics.clear() end
function volta.graphics.drawRect(x: number, y: number, width: number, height: number, filled: boolean?) end
function volta.graphics.drawCircle(x: number, y: number, radius: number, filled: boolean?) end
function volta.graphics.loadImage(imagePath: string): number
    return 0
end
function volta.graphics.setTextureScaleMode(textureId: number, scaleMode: string) end
function volta.graphics.unloadImage(textureId: number) end
function volta.graphics.drawImage(textureId: number, x: number, y: number, width: number, height: number) end
function volta.graphics.drawImageRegion(textureId: number, x: number, y: number, width: number, height: number, regionX: number, regionY: number, regionWidth: number, regionHeight: number) end
function volta.graphics.loadFont(fontPath: string, fontSize: number): number
    return 0
end
function volta.graphics.unloadFont(fontId: number) end
function volta.graphics.drawText(fontId: number, text: string, x: number, y: number, cacheLifetimeFrames: number?) end
function volta.graphics.pushCoord() end
function volta.graphics.translateCoord(x: number, y: number) end
function volta.graphics.scaleCoord(x: number, y: number) end
function volta.graphics.rotateCoord(radians: number) end
function volta.graphics.shearCoord(x: number, y: number) end
function volta.graphics.popCoord() end
function volta.graphics.setScissor(x: number, y: number, width: number, height: number) end
function volta.graphics.getScissor(): number
    return 0, 0, 0, 0
end

volta.input = {}

volta.input.keys = {} :: {string: number}
volta.input.mouseButtons = {} :: {string: number}

function volta.graphics.isKeyDown(keyId: number): boolean
    return false
end
function volta.graphics.isKeyUp(keyId: number): boolean
    return false
end
function volta.graphics.isMouseButtonDown(mouseButtonId: number): boolean
    return false
end
function volta.graphics.isMouseButtonUp(mouseButtonId: number): boolean
    return false
end
function volta.graphics.getMousePosition(): number
    return 0, 0
end
function volta.graphics.getMouseWheelMove(): number
    return 0, 0
end

volta.audio = {}

function volta.audio.loadAudio(audioPath: string): number
    return 0
end
function volta.audio.unloadAudio(audioId: number) end
function volta.audio.getDuration(audioId: number): number
    return 0
end
function volta.audio.play(audioId: number): number
    return 0
end
function volta.audio.stop(trackId: number) end
function volta.audio.pause(trackId: number) end
function volta.audio.resume(trackId: number) end
function volta.audio.getRemaining(trackId: number): number
    return 0
end
function volta.audio.setVolume(trackId: number, volume: number) end
function volta.audio.setPanning(trackId: number, leftVolume: number, rightVolume: number) end
function volta.audio.setMasterVolume(volume: number) end

volta.system = {}

function volta.system.getOS(): string
    return ""
end
function volta.system.getEnv(variableName: string): string
    return ""
end
function volta.system.getClipboardText(): string
    return ""
end
function volta.system.setClipboardText(text: string) end

volta.filesystem = {}

function volta.filesystem.absolute(relativePath: string): string
    return ""
end
function volta.filesystem.getWorkingDir(): string
    return ""
end
function volta.filesystem.getLuauMainDir(): string
    return ""
end
function volta.filesystem.exists(path: string): boolean
    return false
end
function volta.filesystem.isFile(path: string): boolean
    return false
end
function volta.filesystem.isDirectory(path: string): boolean
    return false
end
function volta.filesystem.isSymlink(path: string): boolean
    return false
end
function volta.filesystem.readFile(path: string): string
    return ""
end
function volta.filesystem.writeFile(path: string, data: string) end
function volta.filesystem.delete(path: string) end
function volta.filesystem.rename(path: string, newPath: string) end
function volta.filesystem.list(path: string): {string}
    return {""}
end
function volta.filesystem.createDirs(path: string) end
function volta.filesystem.removeAll(path: string) end
function volta.filesystem.getFileSize(relativePath: string): number
    return 0
end

volta.network = {}

export type Response = {
    status: number,
    reason: string,
    body: string,
    headers: {string: string},
    trailers: {string: string}
}
export type Client = {
    new: (host: string, port: number?) -> (Client),
    stop: (self: Client) -> (),
    isRunning: (self: Client) -> (boolean),
    getHost: (self: Client) -> (string),
    getPort: (self: Client) -> (number),
    setDefaultHeaders: (self: Client, headers: {string: string}) -> (),
    setConnectionTimeout: (self: Client, timeout: number) -> (),
    setReadTimeout: (self: Client, timeout: number) -> (),
    setWriteTimeout: (self: Client, timeout: number) -> (),
    get: (self: Client, path: string, headers: {string: string}?) -> (Response),
    head: (self: Client, path: string, headers: {string: string}?) -> (Response),
    post: (self: Client, path: string, body: string, contentType: string, headers: {string: string}?) -> (Response),
    put: (self: Client, path: string, body: string, contentType: string, headers: {string: string}?) -> (Response),
    patch: (self: Client, path: string, body: string, contentType: string, headers: {string: string}?) -> (Response),
    delete: (self: Client, path: string, body: string?, contentType: string?, headers: {string: string}?) -> (Response)
}
volta.network.Client = {} :: Client

volta.Color = {}
export type Color = {
    r: number,
    g: number,
    b: number,
    a: number,

    new: (r: number, g: number, b: number, alpha: number?) -> (Color),
    fromHex: (hex: string) -> (Color),
    fromHexa: (hexa: string) -> (Color),
    toHex: (self: Color) -> (string),
    toHexa: (self: Color) -> (string),
    lerp: (self: Color, targetColor: Color, progress: number) -> (Color)
}
volta.Color = {} :: Color

volta.Sprite = {}
export type Sprite = {
    x: number,
    y: number,
    w: number,
    h: number,
    textureId: number,
    r: number,
    flipH: boolean,
    flipV: boolean,
    regionX: number,
    regionY: number,
    regionW: number,
    regionH: number,

    new: (x: number, y: number, width: number, height: number, texture: string | number, rotation: number?) -> (Sprite),
    draw: (self: Sprite) -> (),
    setTexture: (self: Sprite, texturePath: string) -> (),
    setTextureId: (self: Sprite, textureId: number) -> (),
    setPosition: (self: Sprite, x: number, y: number) -> (),
    setSize: (self: Sprite, width: number, height: number) -> (),
    setRotation: (self: Sprite, rotation: number) -> (),
    setTextureRegion: (self: Sprite, x: number, y: number, width: number, height: number) -> (),
    flipHorizontal: (self: Sprite, enable: boolean) -> (),
    flipVertical: (self: Sprite, enable: boolean) -> (),
    getTextureId: (self: Sprite) -> (number),
    getPosition: (self: Sprite) -> (number),
    getSize: (self: Sprite) -> (number),
    getRotation: (self: Sprite) -> (number),
    getFlipHorizontal: (self: Sprite) -> (boolean),
    getFlipVertical: (self: Sprite) -> (boolean)
}
volta.Sprite = {} :: Sprite

volta.AnimatedSprite = {}
export type AnimatedSprite = {
    x: number,
    y: number,
    w: number,
    h: number,
    r: number,
    flipH: boolean,
    flipV: boolean,
    frameDuration: number,
    elapsedTime: number,
    currentFrame: number,
    frameCount: number,
    textureId: number?,
    regions: {{number}}?,

    new: (x: number, y: number, width: number, height: number, frameDuration: number, ...number) -> (AnimatedSprite),
    fromSpritesheet: (x: number, y: number, width: number, height: number, spritesheet: spritesheet.Spritesheet, frameDuration: number) -> (AnimatedSprite),
    draw: (self: AnimatedSprite, deltaTime: number) -> (),
    setPosition: (self: AnimatedSprite, x: number, y: number) -> (),
    setSize: (self: AnimatedSprite, width: number, height: number) -> (),
    setRotation: (self: AnimatedSprite, rotation: number) -> (),
    setTextureIds: (self: AnimatedSprite, ...number) -> (),
    setSpritesheet: (self: AnimatedSprite, spritesheet: spritesheet.Spritesheet) -> (),
    setCurrentFrame: (self: AnimatedSprite, frameIndex: number, resetElapsedTime: boolean?) -> (),
    flipHorizontal: (self: AnimatedSprite, enable: boolean) -> (),
    flipVertical: (self: AnimatedSprite, enable: boolean) -> (),
    getTextureId: (self: AnimatedSprite) -> (number),
    getCurrentFrame: (self: AnimatedSprite) -> (number),
    getRegions: (self: AnimatedSprite) -> ({{number}}),
    getPosition: (self: AnimatedSprite) -> (number),
    getSize: (self: AnimatedSprite) -> (number),
    getRotation: (self: AnimatedSprite) -> (number),
    getFlipHorizontal: (self: AnimatedSprite) -> (boolean),
    getFlipVertical: (self: AnimatedSprite) -> (boolean)
}
volta.AnimatedSprite = {} :: AnimatedSprite

return table.freeze(volta)