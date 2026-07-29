#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int absolute(lua_State* L);
int getWorkingDir(lua_State* L);
int getLuauMainDir(lua_State* L);
int exists(lua_State* L);
int isFile(lua_State* L);
int isDirectory(lua_State* L);
int isSymlink(lua_State* L);
int readFile(lua_State* L);
int writeFile(lua_State* L);
int deleteFile(lua_State* L);
int rename(lua_State* L);
int listFiles(lua_State* L);
int createDirs(lua_State* L);
int removeAll(lua_State* L);
void registerFilesystemFunctions(ResourceState* state);

#endif