#ifndef REQUIRE_CONTEXT_H
#define REQUIRE_CONTEXT_H

#include <filesystem>

namespace fs = std::filesystem;

struct RequireContext {
	fs::path scriptsRoot;
	fs::path cursor;
};

#endif