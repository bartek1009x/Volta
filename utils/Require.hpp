#ifndef REQUIRE_H
#define REQUIRE_H

#include "ResourceState.hpp"

#include "../dependencies/luau/VM/include/lua.h"
#include "../dependencies/luau/Require/include/Luau/Require.h"

static void LibRequire_InitConfiguration(luarequire_Configuration *config);
void registerRequireLib(ResourceState* state);

#endif