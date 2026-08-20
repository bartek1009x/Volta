#include "filesystem.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

#include "../dependencies/luau/VM/include/lualib.h"

using namespace std;
namespace fs = filesystem;

static ResourceState* resourceState = nullptr;

int absolute(lua_State *L) {
    const fs::path finalPath = fs::absolute(lua_tostring(L, 1));
    const std::string stringPath = finalPath.string(); // we need to convert it here, because if we don't, path.c_str() will return const wchar_t* on windows (we need const char*)
    const char* pathCStr = stringPath.c_str();
    lua_pushstring(L, pathCStr);
    return 1;
}

int getWorkingDir(lua_State *L) {
    const fs::path finalPath = fs::current_path();
    const std::string stringPath = finalPath.string(); // we need to convert it here, because if we don't, path.c_str() will return const wchar_t* on windows (we need const char*)
    const char* pathCStr = stringPath.c_str();
    lua_pushstring(L, pathCStr);
    return 1;
}

int getLuauMainDir(lua_State *L) {
    const fs::path finalPath = resourceState->getMainPath();
    const std::string stringPath = finalPath.string(); // we need to convert it here, because if we don't, path.c_str() will return const wchar_t* on windows (we need const char*)
    const char* pathCStr = stringPath.c_str();
    lua_pushstring(L, pathCStr);
    return 1;
}

int exists(lua_State *L) {
    lua_pushboolean(L, fs::exists(lua_tostring(L, 1)));
    return 1;
}

int isFile(lua_State *L) {
    lua_pushboolean(L, fs::is_regular_file(lua_tostring(L, 1)));
    return 1;
}

int isDirectory(lua_State *L) {
    lua_pushboolean(L, fs::is_directory(lua_tostring(L, 1)));
    return 1;
}

int isSymlink(lua_State *L) {
    lua_pushboolean(L, fs::is_symlink(lua_tostring(L, 1)));
    return 1;
}

int readFile(lua_State* L) {
    string data;

    ifstream File(lua_tostring(L, 1));

    string line;
    while (getline (File, line)) {
        data += line;
        data += '\n';
    }

    File.close();

    lua_pushstring(L, data.c_str());
    return 1;
}

int writeFile(lua_State* L) {
    ofstream File;
    File.open(lua_tostring(L, 1));
    File << lua_tostring(L, 2);
    File.close();
    return 0;
}

int rename(lua_State* L) {
    fs::rename(lua_tostring(L, 1), lua_tostring(L, 2));
    return 0;
}

int deleteF(lua_State* L) {
    try {
        fs::remove(lua_tostring(L, 1));
    } catch (const fs::filesystem_error& err) {
        cout << "Filesystem error: " << err.what() << '\n';
        luaL_error(L, "Filesystem error: %s", err.what());
    }
    return 0;
}

int createDirs(lua_State *L) {
    fs::path dir = lua_tostring(L, 1);
    fs::create_directories(dir);
    return 0;
}

int removeAll(lua_State *L) {
    fs::remove_all(lua_tostring(L, 1));
    return 0;
}

int list(lua_State *L) {
    lua_createtable(L, 1, 1);

    int i = 1;
    for (const auto & entry : fs::directory_iterator(lua_tostring(L, 1))) {
        const fs::path finalPath = entry.path();
        const std::string stringPath = finalPath.string(); // we need to convert it here, because if we don't, path.c_str() will return const wchar_t* on windows (we need const char*)
        const char* pathCStr = stringPath.c_str();
        lua_pushstring(L, pathCStr);
        lua_rawseti(L, -2, i);
        i++;
    }

    return 1;
}

int getFileSize(lua_State* L) {
    lua_pushnumber(L, fs::file_size(lua_tostring(L, 1)));
    return 1;
}

static const luaL_Reg fs_lib[] = {
    {"absolute", absolute},
    {"getWorkingDir", getWorkingDir},
    {"getLuauMainDir", getLuauMainDir},
    {"exists", exists},
    {"isFile", isFile},
    {"isDirectory", isDirectory},
    {"isSymlink", isSymlink},
    {"readFile", readFile},
    {"writeFile", writeFile},
    {"delete", deleteF},
    {"rename", rename},
    {"list", list},
    {"createDirs", createDirs},
    {"removeAll", removeAll},
    {"getFileSize", getFileSize},
    {nullptr, nullptr},
};

void registerFilesystemFunctions(lua_State* L, ResourceState* state) {
    resourceState = state;

    luaL_register(L, "filesystem", fs_lib);
    lua_setreadonly(L, -1, 1);
    lua_setfield(L, -2, "filesystem");
}