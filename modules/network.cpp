#include "network.hpp"

#include <string>
#include <unordered_map>

#include "../dependencies/luau/VM/include/lualib.h"

static std::unordered_map<int, httplib::Client*> clients;
Uint32 clientIDCounter = 0;

httplib::Headers parseLuauHeaders(lua_State* L, int tableStackIdx) {
    httplib::Headers headers;

    lua_pushnil(L);

    while (lua_next(L, tableStackIdx) != 0) {
        if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
            std::string key = lua_tostring(L, -2);
            std::string val = lua_tostring(L, -1);
            headers.emplace(key, val);
        }

        lua_pop(L, 1);
    }

    return headers;
}

void cppHeadersToLuauTable(lua_State* L, const httplib::Headers& headers) {
    lua_createtable(L, 0, headers.size());

    for (auto it = headers.begin(); it != headers.end();) {
        auto range = headers.equal_range(it->first);

        std::string name = it->first;
        std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        lua_createtable(L, 0, 0);

        int index = 1;
        for (auto valueIt = range.first; valueIt != range.second; ++valueIt) {
            lua_pushlstring(L, valueIt->second.data(), valueIt->second.size());
            lua_rawseti(L, -2, index++);
        }

        lua_setfield(L, -2, name.c_str());
        it = range.second;
    }
}

void pushResponse(lua_State* L, const httplib::Result& res) {
    lua_newtable(L);

    lua_pushnumber(L, res->status);
    lua_setfield(L, -2, "status");

    lua_pushlstring(L, res->body.data(), res->body.size());
    lua_setfield(L, -2, "body");

    lua_pushstring(L, res->reason.c_str());
    lua_setfield(L, -2, "reason");

    cppHeadersToLuauTable(L, res->headers);
    lua_setfield(L, -2, "headers");

    cppHeadersToLuauTable(L, res->trailers);
    lua_setfield(L, -2, "trailers");
}

httplib::Client* getClient(lua_State* L) {
    lua_rawgetfield(L, 1, "id");
    int id = lua_tointeger(L, -1);

    if (clients.find(id) == clients.end()) {
        luaL_error(L, "Could not find the client. Are you sure it's still running?");
        return nullptr;
    }

    httplib::Client* client = clients[id];
    lua_pop(L, 1);

    return client;
}

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
        return 0;
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

int setDefaultHeaders(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    httplib::Headers headers = parseLuauHeaders(L, 2);

    client->set_default_headers(headers);

    return 0;
}

int setConnectionTimeout(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    int milliseconds = lua_tointeger(L, 2);
    time_t seconds = milliseconds / 1000;
    time_t microseconds = (milliseconds % 1000) * 1000;
    client->set_connection_timeout(seconds, microseconds);

    return 0;
}

int setReadTimeout(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    int milliseconds = lua_tointeger(L, 2);
    time_t seconds = milliseconds / 1000;
    time_t microseconds = (milliseconds % 1000) * 1000;
    client->set_read_timeout(seconds, microseconds);

    return 0;
}

int setWriteTimeout(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    int milliseconds = lua_tointeger(L, 2);
    time_t seconds = milliseconds / 1000;
    time_t microseconds = (milliseconds % 1000) * 1000;
    client->set_write_timeout(seconds, microseconds);

    return 0;
}

int get(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    httplib::Result res;
    if (!lua_isnoneornil(L, 3)) {
        httplib::Headers headers = parseLuauHeaders(L, 3);

        res = client->Get(lua_tostring(L, 2), headers);
    } else {
        res = client->Get(lua_tostring(L, 2));
    }

    if (!res) {
        luaL_error(L, to_string(res.error()).c_str());
        return 1;
    }

    pushResponse(L, res);

    return 1;
}

int head(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    httplib::Result res;
    if (!lua_isnoneornil(L, 3)) {
        httplib::Headers headers = parseLuauHeaders(L, 3);

        res = client->Head(lua_tostring(L, 2), headers);
    } else {
        res = client->Head(lua_tostring(L, 2));
    }

    if (!res) {
        luaL_error(L, to_string(res.error()).c_str());
        return 1;
    }

    pushResponse(L, res);

    return 1;
}

int post(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    size_t length;
    const char* data = luaL_checklstring(L, 3, &length);
    std::string requestBody(data, length);

    httplib::Result res;
    if (!lua_isnoneornil(L, 5)) {
        httplib::Headers headers = parseLuauHeaders(L, 5);

        res = client->Post(lua_tostring(L, 2), headers, requestBody, lua_tostring(L, 4));
    } else {
        res = client->Post(lua_tostring(L, 2), requestBody, lua_tostring(L, 4));
    }

    if (!res) {
        luaL_error(L, to_string(res.error()).c_str());
        return 1;
    }

    pushResponse(L, res);

    return 1;
}

int put(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    size_t length;
    const char* data = luaL_checklstring(L, 3, &length);
    std::string requestBody(data, length);

    httplib::Result res;
    if (!lua_isnoneornil(L, 5)) {
        httplib::Headers headers = parseLuauHeaders(L, 5);

        res = client->Put(lua_tostring(L, 2), headers, requestBody, lua_tostring(L, 4));
    } else {
        res = client->Put(lua_tostring(L, 2), requestBody, lua_tostring(L, 4));
    }

    if (!res) {
        luaL_error(L, to_string(res.error()).c_str());
        return 1;
    }

    pushResponse(L, res);

    return 1;
}

int patch(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    size_t length;
    const char* data = luaL_checklstring(L, 3, &length);
    std::string requestBody(data, length);

    httplib::Result res;
    if (!lua_isnoneornil(L, 5)) {
        httplib::Headers headers = parseLuauHeaders(L, 5);

        res = client->Patch(lua_tostring(L, 2), headers, requestBody, lua_tostring(L, 4));
    } else {
        res = client->Patch(lua_tostring(L, 2), requestBody, lua_tostring(L, 4));
    }

    if (!res) {
        luaL_error(L, to_string(res.error()).c_str());
        return 1;
    }

    pushResponse(L, res);

    return 1;
}

int deleteH(lua_State *L) {
    // self at index 1
    httplib::Client* client = getClient(L);
    if (client == nullptr) return 0;

    httplib::Result res;
    if (!lua_isnoneornil(L, 3)) {
        size_t length;
        const char* data = luaL_checklstring(L, 3, &length);
        std::string requestBody(data, length);

        if (!lua_isnoneornil(L, 5)) {
            httplib::Headers headers = parseLuauHeaders(L, 5);

            res = client->Delete(lua_tostring(L, 2), headers, requestBody, lua_tostring(L, 4));
        } else {
            res = client->Delete(lua_tostring(L, 2), requestBody, lua_tostring(L, 4));
        }
    } else {
        res = client->Delete(lua_tostring(L, 2));
    }

    if (!res) {
        luaL_error(L, to_string(res.error()).c_str());
        return 1;
    }

    pushResponse(L, res);

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

    lua_pushcfunction(L, setDefaultHeaders, "setDefaultHeaders");
    lua_setfield(L, -2, "setDefaultHeaders");

    lua_pushcfunction(L, setConnectionTimeout, "setConnectionTimeout");
    lua_setfield(L, -2, "setConnectionTimeout");

    lua_pushcfunction(L, setReadTimeout, "setReadTimeout");
    lua_setfield(L, -2, "setReadTimeout");

    lua_pushcfunction(L, setWriteTimeout, "setWriteTimeout");
    lua_setfield(L, -2, "setWriteTimeout");

    lua_pushcfunction(L, get, "get");
    lua_setfield(L, -2, "get");

    lua_pushcfunction(L, head, "head");
    lua_setfield(L, -2, "head");

    lua_pushcfunction(L, post, "post");
    lua_setfield(L, -2, "post");

    lua_pushcfunction(L, put, "put");
    lua_setfield(L, -2, "put");

    lua_pushcfunction(L, patch, "patch");
    lua_setfield(L, -2, "patch");

    lua_pushcfunction(L, deleteH, "delete");
    lua_setfield(L, -2, "delete");

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