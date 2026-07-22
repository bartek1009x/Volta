#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "dependencies/luau/VM/include/lua.h"
#include "dependencies/luau/VM/include/lualib.h"
#include "dependencies/luau/Compiler/include/luacode.h"

#include "require.cpp"

int main(int argc, char* argv[]) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, luau_require, "require");
    lua_setglobal(L, "require");

    luaL_sandbox(L);

    auto scriptPath = std::filesystem::current_path() / argv[1];
    auto size = std::filesystem::file_size(scriptPath);
    std::string script(size, '\0');
    std::ifstream in(scriptPath);
    in.read(&script[0], size);

    size_t bytecode_size;
    char* bytecode = luau_compile(script.c_str(), script.length(), nullptr, &bytecode_size);
    std::string chunkname = "=" + scriptPath.string();
    int res = luau_load(L, chunkname.c_str(), bytecode, bytecode_size, 0);
    free(bytecode);

    if (res != 0) {
        size_t len;
        const char* msg = lua_tolstring(L, -1, &len);
        lua_pop(L, 1);
        printf("Failed to compile: %s\n", msg);
        return 1;
    }

    lua_State* T = lua_newthread(L);
    lua_pushvalue(L, -2);
    lua_remove(L, -3);
    lua_xmove(L, T, 1);

    int status = lua_resume(T, nullptr, 0);
    if (status != LUA_OK) {
        const char* err = lua_tostring(T, -1);
        printf("Runtime error: %s\n", err ? err : "unknown error");
    }

    lua_close(L);
    return 0;
}