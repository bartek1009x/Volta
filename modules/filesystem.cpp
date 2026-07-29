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
    lua_pushstring(L, fs::absolute(lua_tostring(L, 1)).c_str());
    return 1;
}

int getWorkingDir(lua_State *L) {
    lua_pushstring(L, fs::current_path().c_str());
    return 1;
}

int getLuauMainDir(lua_State *L) {
    lua_pushstring(L, resourceState->getMainPath().c_str());
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

int createDirectories(lua_State *L) {
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
        lua_pushstring(L, entry.path().c_str());
        lua_rawseti(L, -2, i);
        i++;
    }

    return 1;
}

int getFileSize(lua_State* L) {
    lua_pushnumber(L, fs::file_size(lua_tostring(L, 1)));
    return 1;
}

void registerFilesystemFunctions(ResourceState* state) {
    resourceState = state;
    lua_State* L = state->getL();

    lua_createtable(L, 1, 0);

    lua_pushcfunction(L, absolute, "absolute");
    lua_setfield(L, -2, "absolute");
    lua_pushcfunction(L, getWorkingDir, "getWorkingDir");
    lua_setfield(L, -2, "getWorkingDir");
    lua_pushcfunction(L, getLuauMainDir, "getLuauMainDir");
    lua_setfield(L, -2, "getLuauMainDir");
    lua_pushcfunction(L, exists, "exists");
    lua_setfield(L, -2, "exists");
    lua_pushcfunction(L, isFile, "isFile");
    lua_setfield(L, -2, "isFile");
    lua_pushcfunction(L, isDirectory, "isDirectory");
    lua_setfield(L, -2, "isDirectory");
    lua_pushcfunction(L, isSymlink, "isSymlink");
    lua_setfield(L, -2, "isSymlink");
    lua_pushcfunction(L, readFile, "readFile");
    lua_setfield(L, -2, "readFile");
    lua_pushcfunction(L, writeFile, "writeFile");
    lua_setfield(L, -2, "writeFile");
    lua_pushcfunction(L, deleteF, "delete");
    lua_setfield(L, -2, "delete");
    lua_pushcfunction(L, rename, "rename");
    lua_setfield(L, -2, "rename");
    lua_pushcfunction(L, list, "list");
    lua_setfield(L, -2, "list");
    lua_pushcfunction(L, createDirectories, "createDirectories");
    lua_setfield(L, -2, "createDirectories");
    lua_pushcfunction(L, removeAll, "removeAll");
    lua_setfield(L, -2, "removeAll");
    lua_pushcfunction(L, getFileSize, "getFileSize");
    lua_setfield(L, -2, "getFileSize");

    lua_setglobal(L, "filesystem");
}