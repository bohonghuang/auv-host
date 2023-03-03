//
// Created by Qff on 2023/3/1.
//

#ifndef LUA_H
#define LUA_H

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <lua.hpp>


namespace auv::lua {

class LuaModule {
public:
  LuaModule() noexcept = default;
  LuaModule(std::string_view name, std::initializer_list<luaL_Reg> funcs) noexcept;
  void add_function(const luaL_Reg &func) noexcept;

  const std::vector<luaL_Reg> &get_lua_function_table() const noexcept;
  const std::string &get_name() const noexcept;

private:
  std::string m_name;
  std::vector<luaL_Reg> m_functions;
};

class Lua {
public:
  Lua();
  ~Lua();

  bool load_file(std::string_view file_name) noexcept;// error return false
  void add_function(std::string_view func_name, lua_CFunction func);
  void add_function(std::string_view module_name, std::string_view func_name, lua_CFunction func);
  void add_module(const LuaModule &module) noexcept;
  std::string do_string(std::string_view string);

  template<typename T>
  T get_global_var(std::string_view var_name) {
    lua_getglobal(m_lua_state, var_name.data());
    T t = get_var<T>();
    lua_pop(m_lua_state, 1);
    return t;
  }

  template<typename Type, typename Name>
  Type get_table_var(std::string_view table_name, Name name) {
    lua_getglobal(m_lua_state, table_name.data());
    Type t;
    lua_getfield(m_lua_state, -1, name);
    t = get_var<Type>();
    lua_pop(m_lua_state, 2);
    return t;
  }

  template<typename... Type, typename... Name>
  std::tuple<Type...> get_table_var(std::string_view table_name, Name... name) {
    static_assert(std::is_same_v<std::common_type_t<Name...>, const char *>);
    lua_getglobal(m_lua_state, table_name.data());
    std::tuple<Type...> tuple;
    std::apply(
        [this, name...](Type &...args) {
          (..., (lua_getfield(this->m_lua_state, -1, name), args = this->get_var<Type>(), lua_pop(this->m_lua_state, 1)));
        },
        tuple);
    lua_pop(m_lua_state, 1);
    return tuple;
  }

  template<typename T>
  void set_global_var(std::string_view var_name, T val) {
    push_var(val);
    lua_setglobal(m_lua_state, var_name.data());
  }

  template<typename... Type>
  void set_table_var(std::string_view table_name, std::array<const char *, sizeof...(Type)> keys, Type... values) {
    lua_getglobal(m_lua_state, table_name.data());
    if (lua_isnil(m_lua_state, -1)) {
      lua_pop(m_lua_state, 1);
      lua_newtable(m_lua_state);
    }
    std::apply(
        [this, values...](const auto &...key) {
          (..., (push_var(values), lua_setfield(m_lua_state, -2, key)));
        },
        keys);
    lua_setglobal(m_lua_state, table_name.data());
  }

  template<typename T>
  void set_table_var(std::string_view table_name, const char *name, T value) {
    set_table_var(table_name, std::tuple<const char *>{name}, std::tuple<T>{value});
  }

  template<typename... Results, typename... Args>
  std::tuple<Results...> call(std::string_view func_name, Args... args) {
    std::tuple<Results...> results;
    lua_getglobal(m_lua_state, func_name.data());
    (push_var(args), ...);
    lua_pcall(m_lua_state, sizeof...(Args), sizeof...(Results), 1);
    std::apply(
        [this](auto &...result) {
          ((result = get_var<Results>()), ...);
          lua_pop(m_lua_state, 1);
        },
        results);
    return results;
  }

private:
  template<typename T>
  void push_var(T val) {
    return push_var(m_lua_state, val);
  }

  template<typename T>
  T get_var(int index = -1) {
    return get_var<T>(m_lua_state, index);
  }

public:
  lua_State *m_lua_state;
  std::map<std::string, LuaModule> m_modules;
};

template<class T>
struct dependent_false : std::false_type {};

template<typename T>
void push_var(lua_State *m_lua_state, T val) {
  if constexpr (std::is_same_v<T, std::string>)
    lua_pushstring(m_lua_state, val.c_str());
  else if constexpr (std::is_same_v<T, bool>)
    lua_pushboolean(m_lua_state, val);
  else if constexpr (std::is_arithmetic_v<T>)
    lua_pushnumber(m_lua_state, static_cast<double>(val));
  else if constexpr (std::is_pointer_v<T>)
    lua_pushlightuserdata(m_lua_state, const_cast<void *>(reinterpret_cast<const void *>(val)));
}

template<typename T>
T get_var(lua_State *m_lua_state, int index = -1) {
  T t{};
  if constexpr (std::is_same_v<T, std::string>) {
    if (!lua_isstring(m_lua_state, index)) {
      lua_pop(m_lua_state, 2);
      throw std::logic_error("The var is not string type.");
    }
    t.append(lua_tostring(m_lua_state, index));
  } else if constexpr (std::is_same_v<T, bool>) {
    if (!lua_isboolean(m_lua_state, index)) {
      lua_pop(m_lua_state, 2);
      throw std::logic_error("The var is not bool type.");
    }
    t = static_cast<T>(lua_toboolean(m_lua_state, index));
  } else if constexpr (std::is_arithmetic_v<T>) {
    if (!lua_isnumber(m_lua_state, index)) {
      lua_pop(m_lua_state, 2);
      throw std::logic_error("The var is not number type.");
    }
    t = static_cast<T>(lua_tonumber(m_lua_state, index));
  } else if constexpr (std::is_pointer_v<T>) {
    if (!lua_isuserdata(m_lua_state, index)) {
      lua_pop(m_lua_state, 2);
      throw std::logic_error("The var is not userdata.");
    }
    t = reinterpret_cast<T *>(lua_touserdata(m_lua_state, index));
  } else {
    static_assert(dependent_false<T>(), "Illegal return type for get_var.");
  }
  return t;
}

template<class T>
struct do_reversely {
  T t;
  template<class F>
  do_reversely(F f) {
    f();
  }
  template<class U>
  do_reversely<T> operator=(do_reversely<U>) {
    return *this;
  }
};

template<auto f>
struct lua_function_impl {};

template<auto f>
struct tupled_function_pointer_impl {};

template<class... T0, class R0, R0(*unwrapped)(T0...)>
struct tupled_function_pointer_impl<unwrapped> {
  template<class R>
  struct inner {
    template<class... T>
    constexpr static std::tuple<R> (*transformed)(T...) = [](T... arg) -> std::tuple<R> {
      return {unwrapped(arg...)};
    };
  };

  template<class... R>
  struct inner<std::tuple<R...>> {
    template<class... T>
    constexpr static std::tuple<R...> (*transformed)(T...) = unwrapped;
  };
  
  constexpr static auto transformed = inner<R0>::template transformed<T0...>;
};

template<auto f>
constexpr static auto tupled_function_pointer = tupled_function_pointer_impl<f>::transformed;

template<auto f>
constexpr lua_CFunction lua_function = lua_function_impl<tupled_function_pointer<f>>::wrapped;

template<class... T, class... R, std::tuple<R...>(*unwrapped)(T...)>
struct lua_function_impl<unwrapped> {
  static constexpr lua_CFunction wrapped = [](lua_State *l) -> int {
    std::tuple<T...> arg_tuple;
    std::apply([l](T &...arg) {
      (do_reversely<T>{[l, &arg] {
         arg = get_var<T>(l);
         lua_pop(l, 1);
       }} = ...);
    }, arg_tuple);
    return std::apply([l](T... arg) {
      std::apply([l](R... ret) {
        (push_var(l, ret), ...);
      }, unwrapped(arg...));
      return sizeof...(R);
    }, arg_tuple);
  };
};

template<auto f>
void push_cppfunction_impl(lua_State *l, const char* name) {
  lua_pushcfunction(l, lua_function<f>);
  lua_setglobal(l, name);
}

#define push_cppfunction(l, f) push_cppfunction_impl<f>(l, #f)
#define push_named_cppfunction(l, name, f) push_cppfunction_impl<f>(l, name)

}// namespace auv::lua

#endif//LUA_H
