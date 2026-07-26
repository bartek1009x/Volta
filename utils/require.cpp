// this code was written by Claude
//
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "../dependencies/luau/VM/include/lua.h"
#include "../dependencies/luau/VM/include/lualib.h"
#include "../dependencies/luau/Compiler/include/luacode.h"

#include "ExecutablePath.hpp"

using namespace std;
static unordered_map<string, int> g_moduleCache;
static const filesystem::path& stdlibRoot() {
    static filesystem::path root = getExecutableDir() / "extras";
    return root;
}
static int luau_require(lua_State* L) {
    const char* rawPath = luaL_checkstring(L, 1);
    string rawStr(rawPath);

    static const string kInternalPrefix = "@volta/";
    filesystem::path resolved;

    if (rawStr.rfind(kInternalPrefix, 0) == 0) {
        // require("@volta/json") -> <exeDir>/extras/json.luau
        string moduleName = rawStr.substr(kInternalPrefix.size());
        resolved = filesystem::weakly_canonical(stdlibRoot() / moduleName);
    } else {
        // Every other require -- "foo", "./foo", "../foo" -- is resolved
        // relative to the calling script's directory, same as an explicit "./".
        lua_Debug ar;
        lua_getinfo(L, 1, "s", &ar);
        filesystem::path callerDir = (ar.source && ar.source[0])
            ? filesystem::path(ar.source + 1).parent_path()
            : filesystem::current_path();
        resolved = filesystem::weakly_canonical(callerDir / rawStr);
    }

    resolved += ".luau";
    string key = resolved.string();
    auto it = g_moduleCache.find(key);
    if (it != g_moduleCache.end()) {
        lua_getref(L, it->second);
        return 1;
    }
    if (!filesystem::exists(resolved))
        luaL_error(L, "module not found: %s", key.c_str());
    auto size = filesystem::file_size(resolved);
    string src(size, '\0');
    ifstream in(resolved);
    in.read(&src[0], size);
    size_t bytecodeSize;
    char* bytecode = luau_compile(src.c_str(), src.size(), nullptr, &bytecodeSize);
    string chunkname = "=" + key;
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