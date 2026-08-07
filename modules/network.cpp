#include "network.hpp"

#include <string>
#include <unordered_map>

#include "../dependencies/luau/VM/include/lualib.h"

static std::unordered_map<int, httplib::Client*> clients;
Uint32 clientIDCounter = 0;

int clientNew(lua_State *L) {
    std::string host = lua_tostring(L, 1);
    int port = luaL_optnumber(L, 2, -1);

    httplib::Client* client;
    if (port != -1) {
        client = new httplib::Client(host, port);
    } else {
        client = new httplib::Client(host);
        port = client->port();
    }

    lua_newtable(L);

    lua_pushnumber(L, clientIDCounter);
    lua_setfield(L, -2, "id");

    lua_pushstring(L, host.c_str());
    lua_setfield(L, -2, "host");

    lua_pushnumber(L, port);
    lua_setfield(L, -2, "port");

    lua_pushboolean(L, true);
    lua_setfield(L, -2, "running");

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);

    clients[clientIDCounter] = client;
    clientIDCounter++;

    return 1;
}

int stopC(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "id");
    int id = lua_tointeger(L, -1);

    if (clients.find(id) == clients.end()) {
        luaL_error(L, "Could not find the client. Are you sure it's still running?");
    }

    httplib::Client* client = clients[id];
    lua_pop(L, 1);

    client->stop();

    delete client;
    clients.erase(id);

    lua_pushboolean(L, false);
    lua_setfield(L, 1, "running");

    return 0;
}

int isRunning(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "running");
    return 1;
}

int getHost(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "host");
    return 1;
}

int getPort(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "port");
    return 1;
}

int get(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "id");
    int id = lua_tointeger(L, -1);

    if (clients.find(id) == clients.end()) {
        luaL_error(L, "Could not find the client. Are you sure it's still running?");
    }

    httplib::Client* client = clients[id];
    lua_pop(L, 1);
    auto res = client->Get(lua_tostring(L, 2));

    if (!res) {
        lua_pushnil(L);
        return 1;
    }

    int status = res->status;
    std::string body = res->body;
    std::string reason = res->reason;

    lua_newtable(L);

    lua_pushnumber(L, status);
    lua_setfield(L, -2, "status");

    lua_pushstring(L, body.c_str());
    lua_setfield(L, -2, "body");

    lua_pushstring(L, reason.c_str());
    lua_setfield(L, -2, "reason");

    return 1;
}

int post(lua_State *L) {
    // self at index 1
    lua_rawgetfield(L, 1, "id");
    int id = lua_tointeger(L, -1);

    if (clients.find(id) == clients.end()) {
        luaL_error(L, "Could not find the client. Are you sure it's still running?");
    }

    httplib::Client* client = clients[id];
    lua_pop(L, 1);
    auto res = client->Post(lua_tostring(L, 2), lua_tostring(L, 3), lua_tostring(L, 4));

    if (!res) {
        lua_pushnil(L);
        return 1;
    }

    int status = res->status;
    std::string body = res->body;
    std::string reason = res->reason;

    lua_newtable(L);

    lua_pushnumber(L, status);
    lua_setfield(L, -2, "status");

    lua_pushstring(L, body.c_str());
    lua_setfield(L, -2, "body");

    lua_pushstring(L, reason.c_str());
    lua_setfield(L, -2, "reason");

    return 1;
}

void pushClientClass(lua_State *L) {
    lua_newtable(L);

    lua_pushvalue(L, -1);

    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushvalue(L, -1);
    lua_pushcclosure(L, clientNew, "clientNew", 1);
    lua_setfield(L, -2, "new");

    lua_pushcfunction(L, isRunning, "isRunning");
    lua_setfield(L, -2, "isRunning");

    lua_pushcfunction(L, stopC, "stop");
    lua_setfield(L, -2, "stop");

    lua_pushcfunction(L, getHost, "getHost");
    lua_setfield(L, -2, "getHost");

    lua_pushcfunction(L, getPort, "getPort");
    lua_setfield(L, -2, "getPort");

    lua_pushcfunction(L, get, "get");
    lua_setfield(L, -2, "get");

    lua_pushcfunction(L, post, "post");
    lua_setfield(L, -2, "post");

    // class table remains on stack
}

void registerNetworkFunctions(ResourceState* state) {
    lua_State* L = state->getL();

    lua_newtable(L);

    pushClientClass(L);
    lua_setfield(L, -2, "Client");

    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "network");
}