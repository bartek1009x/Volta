// this code was written by Claude

#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "dependencies/luau/VM/include/lua.h"
#include "dependencies/luau/VM/include/lualib.h"
#include "dependencies/luau/Compiler/include/luacode.h"

static std::unordered_map<std::string, int> g_moduleCache;

static int luau_require(lua_State* L) {
    const char* rawPath = luaL_checkstring(L, 1);

    lua_Debug ar;
    lua_getinfo(L, 1, "s", &ar);
    std::filesystem::path callerDir = (ar.source && ar.source[0])
        ? std::filesystem::path(ar.source + 1).parent_path()
        : std::filesystem::current_path();

    std::filesystem::path resolved = std::filesystem::weakly_canonical(callerDir / rawPath);
    resolved += ".luau";

    std::string key = resolved.string();
    auto it = g_moduleCache.find(key);
    if (it != g_moduleCache.end()) {
        lua_getref(L, it->second);
        return 1;
    }

    if (!std::filesystem::exists(resolved))
        luaL_error(L, "module not found: %s", key.c_str());

    auto size = std::filesystem::file_size(resolved);
    std::string src(size, '\0');
    std::ifstream in(resolved);
    in.read(&src[0], size);

    size_t bytecodeSize;
    char* bytecode = luau_compile(src.c_str(), src.size(), nullptr, &bytecodeSize);
    std::string chunkname = "=" + key;
    int loadResult = luau_load(L, chunkname.c_str(), bytecode, bytecodeSize, 0);
    free(bytecode);

    if (loadResult != 0)
        lua_error(L);

    lua_call(L, 0, 1);

    lua_pushvalue(L, -1);
    g_moduleCache[key] = lua_ref(L, -1);
    lua_pop(L, 1);

    return 1;
}