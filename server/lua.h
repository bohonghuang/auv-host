//
// Created by Qff on 2023/3/1.
//

#ifndef LUA_H
#define LUA_H

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <functional>
#include <string_view>

#include <lua.hpp>


namespace auv::lua
{

struct LuaFunction
{
	const char *name;
	lua_CFunction func;
};

class LuaModule
{
public:
	LuaModule() noexcept;
	LuaModule(std::initializer_list<LuaFunction> funcs) noexcept;
	void add_function(LuaFunction func) noexcept;

	const luaL_Reg *get_lua_function_table() noexcept;
private:
	std::vector<luaL_Reg> m_functions;
};

class Lua
{
public:
	Lua();

	bool load_file(std::string_view file_name) noexcept;
	bool add_module(const LuaModule &module) noexcept;

	template<typename T>
	T get_global_var(std::string_view var_name)
	{
		lua_getglobal(m_lua_state, var_name.data());
		T t = get_top_var<T>();
		lua_pop(m_lua_state, 1);
		return std::move(t);
	}

	template<typename ...Type, typename... Name>
	std::tuple<Type...> get_table_var(std::string_view table_name, Name... name)
	{
		lua_getglobal(m_lua_state, table_name.data());
		std::tuple<Type...> tuple;
		std::apply([this, name...](Type&... args){
			(..., (lua_getfield(this->m_lua_state, -1, name), args = this->get_top_var<Type>(), lua_pop(this->m_lua_state, 1)));
		}, tuple);
		lua_pop(m_lua_state, 1);
		return std::move(tuple);
	}
private:
	template<class T> struct dependent_false : std::false_type {};
	template<typename T>
	T get_top_var()
	{
		T t{};
		if constexpr (std::is_same_v<T, std::string>) {
			if (!lua_isstring(m_lua_state, -1))
				throw std::logic_error("The var is not string type.");
			t.append(lua_tostring(m_lua_state, -1));
		}
		else if constexpr (std::is_same_v<T, bool>) {
			if (!lua_isboolean(m_lua_state, -1))
				throw std::logic_error("The var is not bool type.");
			t = static_cast<T>(lua_toboolean(m_lua_state, -1));
		}
		else if constexpr (std::is_arithmetic_v<T>) {
			if (!lua_isnumber(m_lua_state, -1))
				throw std::logic_error("The var is not number type.");
			t = static_cast<T>(lua_tonumber(m_lua_state, -1));
		}
		else if constexpr (std::is_pointer_v<T>) {
			if (!lua_isuserdata(m_lua_state, -1))
				throw std::logic_error("The var is not userdata.");
			t = *reinterpret_cast<T**>(lua_touserdata(m_lua_state, -1));
		}
		else {
			static_assert(dependent_false<T>(), "Illegal return type for get_top_var.");
		}
		return t;
	}
private:
	lua_State *m_lua_state;
	std::map<std::string, LuaModule> m_modules;
};

}

#endif //LUA_H
