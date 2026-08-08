#ifndef NETWORK_H
#define NETWORK_H

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "../dependencies/cpp-httplib/httplib.h"
#include "../dependencies/luau/VM/include/lua.h"
#include "../utils/ResourceState.hpp"

int clientNew(lua_State *L);
int stopC(lua_State *L);
int isRunning(lua_State *L);
int getHost(lua_State *L);
int getPort(lua_State *L);
int setDefaultHeaders(lua_State *L);
int setConnectionTimeout(lua_State *L);
int setReadTimeout(lua_State *L);
int setWriteTimeout(lua_State *L);
int get(lua_State *L);
int head(lua_State *L);
int post(lua_State *L);
int put(lua_State *L);
int patch(lua_State *L);
int deleteH(lua_State *L);
void pushClientClass(lua_State* L);
void registerNetworkFunctions(ResourceState* state);

#endif