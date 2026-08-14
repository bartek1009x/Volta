// written like 99% with AI

#include "Require.hpp"

#include <fstream>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <cstring>

#include "../dependencies/luau/VM/include/lualib.h"
#include "../dependencies/luau/Compiler/include/luacode.h"
#include "../dependencies/luau/CodeGen/include/Luau/CodeGen.h"

#include "RequireContext.hpp"

namespace fs = std::filesystem;

static luarequire_WriteResult CopyStringToBuffer(const std::string &str, char *buffer, size_t bufferSize, size_t *outputSize) {
	*outputSize = str.size() + 1;
	if (*outputSize > bufferSize) {
		return WRITE_BUFFER_TOO_SMALL;
	}
	std::memcpy(buffer, str.c_str(), *outputSize);
	return WRITE_SUCCESS;
}

static std::optional<fs::path> ResolveModuleFile(const fs::path &cursor) {
	std::error_code ec;

	fs::path asFile = cursor;
	asFile += ".luau";
	if (fs::is_regular_file(asFile, ec))
		return asFile;

	fs::path asDir = cursor / "init.luau";
	if (fs::is_regular_file(asDir, ec))
		return asDir;

	return std::nullopt;
}

static RequireContext *AsCtx(void *ctx) {
	return static_cast<RequireContext*>(ctx);
}

static void LibRequire_InitConfiguration(luarequire_Configuration *config) {

	config->is_require_allowed = [](lua_State *L, void *ctx, const char *requirer_chunkname) -> bool {
		std::string_view name = requirer_chunkname;
		return !name.empty() && name.front() == '@';
	};

	config->reset = [](lua_State *L, void *ctx, const char *requirer_chunkname) -> luarequire_NavigateResult {
		RequireContext *rc = AsCtx(ctx);
		std::string_view name = requirer_chunkname;
		if (name.empty() || name.front() != '@')
			return NAVIGATE_NOT_FOUND;

		fs::path requirerPath(std::string(name.substr(1)));
		if (requirerPath.extension() == ".luau")
			requirerPath.replace_extension();

		std::error_code ec;
		fs::path rel = fs::relative(requirerPath, rc->scriptsRoot, ec);
		if (ec || rel.empty() || *rel.begin() == "..")
			return NAVIGATE_NOT_FOUND;

		rc->cursor = requirerPath;
		return NAVIGATE_SUCCESS;
	};

	config->jump_to_alias = [](lua_State *L, void *ctx, const char *path) -> luarequire_NavigateResult {
		return NAVIGATE_NOT_FOUND;
	};

	config->to_parent = [](lua_State *L, void *ctx) -> luarequire_NavigateResult {
		RequireContext *rc = AsCtx(ctx);
		if (rc->cursor == rc->scriptsRoot)
			return NAVIGATE_NOT_FOUND;
		rc->cursor = rc->cursor.parent_path();
		return NAVIGATE_SUCCESS;
	};

	config->to_child = [](lua_State *L, void *ctx, const char *name) -> luarequire_NavigateResult {
		RequireContext *rc = AsCtx(ctx);
		rc->cursor /= name;
		return NAVIGATE_SUCCESS;
	};

	config->is_module_present = [](lua_State *L, void *ctx) -> bool {
		RequireContext *rc = AsCtx(ctx);
		return ResolveModuleFile(rc->cursor).has_value();
	};

	config->get_chunkname = [](lua_State *L, void *ctx, char *buffer, size_t buffer_size, size_t *size_out) -> luarequire_WriteResult {
		RequireContext *rc = AsCtx(ctx);
		auto file = ResolveModuleFile(rc->cursor);
		if (!file)
			return WRITE_FAILURE;
		return CopyStringToBuffer("@" + file->generic_string(), buffer, buffer_size, size_out);
	};

	config->get_loadname = [](lua_State *L, void *ctx, char *buffer, size_t buffer_size, size_t *size_out) -> luarequire_WriteResult {
		RequireContext *rc = AsCtx(ctx);
		auto file = ResolveModuleFile(rc->cursor);
		if (!file)
			return WRITE_FAILURE;
		return CopyStringToBuffer(file->generic_string(), buffer, buffer_size, size_out);
	};

	config->get_cache_key = [](lua_State *L, void *ctx, char *buffer, size_t buffer_size, size_t *size_out) -> luarequire_WriteResult {
		RequireContext *rc = AsCtx(ctx);
		auto file = ResolveModuleFile(rc->cursor);
		if (!file)
			return WRITE_FAILURE;
		std::error_code ec;
		fs::path canon = fs::weakly_canonical(*file, ec);
		return CopyStringToBuffer((ec ? *file : canon).generic_string(), buffer, buffer_size, size_out);
	};

	config->get_config_status = [](lua_State *L, void *ctx) -> luarequire_ConfigStatus {
		RequireContext *rc = AsCtx(ctx);
		std::error_code ec;
		if (fs::is_regular_file(rc->cursor / ".luaurc", ec))
			return CONFIG_PRESENT_JSON;
		return CONFIG_ABSENT;
	};

	config->get_config = [](lua_State *L, void *ctx, char *buffer, size_t buffer_size, size_t *size_out) -> luarequire_WriteResult {
		RequireContext *rc = AsCtx(ctx);
		std::ifstream in(rc->cursor / ".luaurc", std::ios::binary);
		if (!in)
			return WRITE_FAILURE;
		std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return CopyStringToBuffer(contents, buffer, buffer_size, size_out);
	};

	config->load = [](lua_State *L, void *ctx, const char *path, const char *chunkname, const char *loadname) -> int {
		std::ifstream in(loadname);
		if (!in) {
			luaL_error(L, "could not open module '%s'", loadname);
		}
		std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

		size_t bytecodeSize = 0;
		char *bytecode = luau_compile(source.data(), source.size(), nullptr, &bytecodeSize);
		int compileResult = luau_load(L, chunkname, bytecode, bytecodeSize, 0);
		free(bytecode);

		if (compileResult != 0) {
    		luaL_error(L, "could not compile module '%s'", path);
		} else if (Luau::CodeGen::isSupported()) {
            Luau::CodeGen::CompilationOptions native_opts{};

            native_opts.flags = Luau::CodeGen::CodeGenFlags::CodeGen_OnlyNativeModules;

            Luau::CodeGen::CompilationResult res = Luau::CodeGen::compile(L, -1, native_opts);
            if (res.hasErrors()) {
                printf("%s native compilation failed: %d\n", path, res.result);
            }
        }

		int base = lua_gettop(L) - 1;
		lua_pushvalue(L, 6);
		lua_pcall(L, 1, LUA_MULTRET, 0);

		return lua_gettop(L) - base;
	};
}

void registerRequireLib(ResourceState *state) {
	lua_State *L = state->getL();

	const std::filesystem::path scriptsRoot = state->getMainPath();
	auto *rc = new RequireContext{fs::weakly_canonical(scriptsRoot), fs::weakly_canonical(scriptsRoot)};
	state->requireContext.reset(rc);

	luaopen_require(L, LibRequire_InitConfiguration, rc);
}