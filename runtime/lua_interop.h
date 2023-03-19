#ifndef AUV_HOST_RUNTIME_LUA_INTEROP_H
#define AUV_HOST_RUNTIME_LUA_INTEROP_H

#include <any>

#include <sol/sol.hpp>

#include "block.h"

namespace auv::lua {

template<class T>
constexpr static auto object_to_any_function = [](T obj) -> std::any { return obj; };

template<class T>
constexpr static auto object_from_any_function = [](std::any obj) -> T { return std::any_cast<T>(obj); };

constexpr const char *type_name_without_namespace(const char *name) {
  const char *result = name;
  char prev_char = '\0';
  while (*name != '\0') {
    if (*name == ':' && prev_char == ':') result = name + 1;
    prev_char = *name++;
  }
  return result;
}

#define AUV_NEW_SOL_TYPE(TABLE, TYPE, ...)                                                                      \
  {                                                                                                             \
    TABLE.new_usertype<TYPE>(auv::lua::type_name_without_namespace(#TYPE), ##__VA_ARGS__);                      \
    TABLE[auv::lua::type_name_without_namespace(#TYPE)]["from_any"] = auv::lua::object_from_any_function<TYPE>; \
    TABLE[auv::lua::type_name_without_namespace(#TYPE)]["to_any"] = auv::lua::object_to_any_function<TYPE>;     \
  }

template<class T, class = decltype(&T::process), class = void>
struct block_process_overload_impl {
  static auto get() {
    return &T::process;
  }
};

template<class T, class R, class A, class N>
struct block_process_overload_impl<T, R (N::*)(A), std::enable_if_t<std::is_same_v<A, std::any> || std::is_same_v<A, auv::unit_t>>> {
  static auto get() {
    return sol::overload([](T &self) -> R { return self.process(unit_t{}); }, &T::process);
  }
};

template<class T, class R, class A, class N>
struct block_process_overload_impl<T, R (N::*)(A) noexcept, std::enable_if_t<std::is_same_v<A, std::any> || std::is_same_v<A, auv::unit_t>>> {
  static auto get() {
    return block_process_overload_impl<T, R (N::*)(A)>::get();
  }
};

template<class T>
auto block_process_overload() {
  return block_process_overload_impl<T>::get();
}

#define AUV_BLOCK_SOL_METHODS(TYPE) "process", auv::lua::block_process_overload<TYPE>(), "as_untyped", &TYPE::as_untyped

void setup_env(sol::state &state);

extern void (*setup_env_all)(sol::state &);

}// namespace auv::lua

#endif//AUV_HOST_RUNTIME_LUA_INTEROP_H
