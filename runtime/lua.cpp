//
// Created by Qff on 2023/3/1.
//

#include "lua.h"

#include <iostream>

namespace auv::lua {

static int wrap_exceptions(lua_State *L, lua_CFunction f) {
  try {
    return f(L);
  } catch (const char *s) {
    lua_pushstring(L, s);
  } catch (std::exception &e) {
    lua_pushstring(L, e.what());
  } catch (...) {
    lua_pushstring(L, "caught (...)");
  }
  return lua_error(L);
}

static int error_handle(lua_State *L) {
  // #1 = err
  lua_getglobal(L, "debug");       // #2 = debug
  lua_getfield(L, -1, "traceback");// #3 = debug.traceback
  lua_remove(L, 2);                // #2 = debug.traceback
  lua_pushvalue(L, 1);             // #3 = 1
  lua_call(L, 1, 1);               // #2 =  err_str
  const char *err_str = lua_tostring(L, -1);
  std::cout << err_str << std::endl;
  return 1;
}

Lua::Lua() {
  m_lua_state = luaL_newstate();
  luaJIT_setmode(m_lua_state, 0, LUAJIT_MODE_ENGINE);

  lua_pushlightuserdata(m_lua_state, (void *) wrap_exceptions);
  luaJIT_setmode(m_lua_state, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
  lua_pop(m_lua_state, 1);

  luaL_openlibs(m_lua_state);
  lua_pushcfunction(m_lua_state, error_handle);
}

Lua::~Lua() {
  lua_close(m_lua_state);
}

bool Lua::load_file(std::string_view file_name) noexcept {
  int rt = luaL_loadfile(m_lua_state, file_name.data());
  return rt == 0;
}

std::string Lua::do_string(std::string_view str) {
  bool point_flag = false;
  if (!str.empty() && str[0] == '=') {
    point_flag = true;
    str = str.substr(1, str.size() - 1);
  }

  int rt = luaL_dostring(m_lua_state, str.data());
  if (rt == LUA_OK) {
    if (point_flag && lua_isstring(m_lua_state, -1)) {
      return lua_tostring(m_lua_state, -1);
    }
    return {};
  }

  std::string err = lua_tostring(m_lua_state, -1);
  lua_pop(m_lua_state, 1);

  if (rt == LUA_YIELD)
    return err;
  else if (rt == LUA_ERRRUN)
    return "Lua occur a runtime error.   " + err;
  else if (rt == LUA_ERRMEM)
    return "Lua occur memory allocation error. For such errors, Lua does not call the message handler.   " + err;
  else if (rt == LUA_ERRERR)
    return "Lua occur error while running the message handler.   " + err;
  else if (rt == LUA_ERRSYNTAX)
    return "Lua syntax error during precompilation.  " + err;
  else
    return "Lua occur a unknown error.  " + err;
}

void Lua::add_function(std::string_view module_name, std::string_view func_name, lua_CFunction func) {
  lua_getglobal(m_lua_state, module_name.data());
  if (lua_isnil(m_lua_state, -1)) {
    lua_pop(m_lua_state, 1);
    lua_newtable(m_lua_state);
  }

  lua_pushcfunction(m_lua_state, func);
  lua_setfield(m_lua_state, -2, func_name.data());
  lua_setglobal(m_lua_state, module_name.data());
}
void Lua::add_function(std::string_view func_name, lua_CFunction func) {
  lua_getglobal(m_lua_state, "_G");
  lua_pushcfunction(m_lua_state, func);
  lua_setfield(m_lua_state, -2, func_name.data());
  lua_pop(m_lua_state, 1);
}

void Lua::add_module(const LuaModule &mod) noexcept
{
	lua_getglobal(m_lua_state, mod.get_name().c_str());
	if (lua_isnil(m_lua_state, -1)) {
		lua_pop(m_lua_state, 1);
		lua_newtable(m_lua_state);
	}

	const auto& func_table = mod.get_lua_function_table();
	for(const auto& i : func_table) {
		lua_pushcfunction(m_lua_state, i.func);
		lua_setfield(m_lua_state, -2, i.name);
	}

	lua_setglobal(m_lua_state, mod.get_name().c_str());
}

LuaModule::LuaModule(std::string_view name, std::initializer_list<luaL_Reg> funcs) noexcept
    : m_name(name) {
  m_functions.insert(m_functions.begin(), funcs.begin(), funcs.end());
}

void LuaModule::add_function(const luaL_Reg &func) noexcept {
  m_functions.push_back(func);
}

const std::vector<luaL_Reg> &LuaModule::get_lua_function_table() const noexcept {
  return m_functions;
}

const std::string &LuaModule::get_name() const noexcept {
  return m_name;
}

}// namespace auv::lua
