#include "color.hpp"

#include <string>

#include "../dependencies/luau/VM/include/lualib.h"

int lerp(lua_State *L);

int toHex(lua_State *L) {
    // self at index 1

    lua_rawgetfield(L, 1, "r");
    lua_rawgetfield(L, 1, "g");
    lua_rawgetfield(L, 1, "b");

    std::stringstream ss;
    ss << "#"
       << std::hex << std::setfill('0')
       << std::setw(2) << lua_tointeger(L, -3)
       << std::setw(2) << lua_tointeger(L, -2)
       << std::setw(2) << lua_tointeger(L, -1);

    lua_pushstring(L, ss.str().c_str());

    return 1;
}

int toHexa(lua_State *L) {
    // self at index 1

    lua_rawgetfield(L, 1, "r");
    lua_rawgetfield(L, 1, "g");
    lua_rawgetfield(L, 1, "b");
    lua_rawgetfield(L, 1, "a");

    std::stringstream ss;
    ss << "#"
       << std::hex << std::setfill('0')
       << std::setw(2) << lua_tointeger(L, -4)
       << std::setw(2) << lua_tointeger(L, -3)
       << std::setw(2) << lua_tointeger(L, -2)
       << std::setw(2) << lua_tointeger(L, -1);

    lua_pushstring(L, ss.str().c_str());

    return 1;
}

void createColor(lua_State *L, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    lua_newtable(L);

    lua_pushinteger(L, r);
    lua_setfield(L, -2, "r");

    lua_pushinteger(L, g);
    lua_setfield(L, -2, "g");

    lua_pushinteger(L, b);
    lua_setfield(L, -2, "b");

    lua_pushinteger(L, a);
    lua_setfield(L, -2, "a");

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);
}

int lerp(lua_State *L) {
    // self at index 1

    lua_rawgetfield(L, 1, "r");
    lua_rawgetfield(L, 1, "g");
    lua_rawgetfield(L, 1, "b");
    lua_rawgetfield(L, 1, "a");

    Uint8 r1 = lua_tointeger(L, -4);
    Uint8 g1 = lua_tointeger(L, -3);
    Uint8 b1 = lua_tointeger(L, -2);
    Uint8 a1 = lua_tointeger(L, -1);

    luaL_checktype(L, 2, LUA_TTABLE);

    lua_rawgetfield(L, 2, "r");
    lua_rawgetfield(L, 2, "g");
    lua_rawgetfield(L, 2, "b");
    lua_rawgetfield(L, 2, "a");

    Uint8 r2 = lua_tointeger(L, -4);
    Uint8 g2 = lua_tointeger(L, -3);
    Uint8 b2 = lua_tointeger(L, -2);
    Uint8 a2 = lua_tointeger(L, -1);

    double t = lua_tonumber(L, 3);

    Uint8 finalR = (1 - t) * r1 + r2;
    Uint8 finalG = (1 - t) * g1 + g2;
    Uint8 finalB = (1 - t) * b1 + b2;
    Uint8 finalA = (1 - t) * a1 + a2;

    createColor(L, finalR, finalG, finalB, finalA);

    return 1;
}

int colorNew(lua_State *L) {
    // 1 = Color table

    Uint8 r = luaL_optinteger(L, 1, 255);
    Uint8 g = luaL_optinteger(L, 2, 255);
    Uint8 b = luaL_optinteger(L, 3, 255);
    Uint8 a = luaL_optinteger(L, 4, 255);

    createColor(L, r, g, b, a);

    return 1;
}

int fromHex(lua_State *L) {
    // 1 = Color table

    lua_newtable(L);

    std::string str(lua_tostring(L, 1));

    if (!str.empty() && str[0] == '#') {
        str = str.substr(1);
    }

    int hex = std::stoi(str, nullptr, 16);

    int r = (hex >> 16) & 0xFF;
    int g = (hex >> 8) & 0xFF;
    int b = hex & 0xFF;

    createColor(L, r, g, b, 255);

    return 1;
}

int fromHexa(lua_State *L) {
    // 1 = Color table

    lua_newtable(L);

    std::string str(lua_tostring(L, 1));

    if (!str.empty() && str[0] == '#') {
        str = str.substr(1);
    }

    unsigned long hexa = std::stoul(str, nullptr, 16);

    int r = (hexa >> 24) & 0xFF;
    int g = (hexa >> 16) & 0xFF;
    int b = (hexa >> 8) & 0xFF;
    int a = hexa & 0xFF;

    createColor(L, r, g, b, a);

    return 1;
}

void push_class(lua_State *L) {
    lua_newtable(L);

    lua_pushvalue(L, -1);

    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushvalue(L, -1);
    lua_pushcclosure(L, colorNew, "colorNew", 1);
    lua_setfield(L, -2, "new");

    lua_pushvalue(L, -1);
    lua_pushcclosure(L, fromHex, "fromHex", 1);
    lua_setfield(L, -2, "fromHex");

    lua_pushvalue(L, -1);
    lua_pushcclosure(L, fromHexa, "fromHexa", 1);
    lua_setfield(L, -2, "fromHexa");

    lua_pushcfunction(L, toHex, "toHex");
    lua_setfield(L, -2, "toHex");

    lua_pushcfunction(L, toHexa, "toHexa");
    lua_setfield(L, -2, "toHexa");

    lua_pushcfunction(L, lerp, "lerp");
    lua_setfield(L, -2, "lerp");

    // class table remains on stack
}

void registerColorObject(ResourceState* state) {
    lua_State* L = state->getL();

    push_class(L);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "Color");
}