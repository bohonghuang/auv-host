#ifndef AUV_HOST_RUNTIME_LUA_INTEROP_H
#define AUV_HOST_RUNTIME_LUA_INTEROP_H

#include <any>

#include <sol/sol.hpp>

#include "block.h"

namespace auv::lua {

template<class T, class = decltype(&T::process), class = void>
struct block_process_overload_impl {
  static auto get() {
    return &T::process;
  }
};

template<class T, class R, class A>
struct block_process_overload_impl<T, R (T::*)(A), std::enable_if_t<std::is_same_v<A, std::any> || std::is_same_v<A, unit_t>>> {
  static auto get() {
    return sol::overload([](T &self) -> R { return self.process(unit_t{}); }, &T::process);
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
