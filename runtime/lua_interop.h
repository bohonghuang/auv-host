#ifndef AUV_HOST_RUNTIME_LUA_INTEROP_H
#define AUV_HOST_RUNTIME_LUA_INTEROP_H

#include <any>

#include <sol/sol.hpp>

#include "block.h"

namespace auv::lua {

template<class T, class = decltype(&T::process), class = void>
struct block_process_overload_impl {
  static auto
  get() {
    return &T::process;
  }
};

template<class T, class R, class A, class N>
struct block_process_overload_impl<T, R (N::*)(A), std::enable_if_t<std::is_same_v<A, std::any> || std::is_same_v<A, auv::unit_t>>> {
  static auto
  get() {
    return sol::overload([](T &self) -> R { return self.process(unit_t{}); }, &T::process);
  }
};

template<class T, class R, class A, class N>
struct block_process_overload_impl<T, R (N::*)(A) noexcept, std::enable_if_t<std::is_same_v<A, std::any> || std::is_same_v<A, auv::unit_t>>> {
  static auto
  get() {
    return block_process_overload_impl<T, R (N::*)(A)>::get();
  }
};

template<class T>
auto block_process_overload() {
  return block_process_overload_impl<T>::get();
}

template<class T>
std::any
object_to_any(T obj) {
  return obj;
}

template<class T>
T object_from_any(std::any obj) {
  return std::any_cast<T>(obj);
}

template<class T>
struct remove_smart_ptr_impl {
  using type = T;
};

template<class T>
using remove_smart_ptr = typename remove_smart_ptr_impl<T>::type;

template<class T>
struct remove_smart_ptr_impl<std::shared_ptr<T>> {
  using type = remove_smart_ptr<T>;
};

template<class T>
struct remove_smart_ptr_impl<std::weak_ptr<T>> {
  using type = remove_smart_ptr<T>;
};

template<class T>
struct remove_smart_ptr_impl<std::unique_ptr<T>> {
  using type = remove_smart_ptr<T>;
};


template<class T, class = void>
struct is_block_type : std::false_type {};

template<class T>
struct is_block_type<T, std::enable_if_t<std::is_base_of_v<Block<typename T::In, typename T::Out>, T>>> : std::true_type {};

static_assert(is_block_type<AnyBlock>());
static_assert(!is_block_type<int>());

template<class T, class Table, class... Arg>
inline void
new_sol_type_impl(Table &&table, const char *name, Arg &&...arg) {
  using Type = remove_smart_ptr<std::decay_t<T>>;
  if constexpr (is_block_type<Type>())
    table.template new_usertype<Type>(name, std::forward<Arg>(arg)..., "process", auv::lua::block_process_overload<Type>(), "as_untyped", &Type::as_untyped);
  else
    table.template new_usertype<Type>(name, std::forward<Arg>(arg)...);
  if constexpr (!std::is_reference_v<T>) {
    table[name]["from_any"] = object_from_any<T>;
    table[name]["to_any"] = object_to_any<T>;
  }
}


constexpr const char *
type_name_without_namespace(const char *name) {
  const char *result = name;
  char prev_char = '\0';
  while (*name != '\0') {
    if (*name == ':' && prev_char == ':') result = name + 1;
    prev_char = *name++;
  }
  return result;
}

#define AUV_NEW_SOL_TYPE(TABLE, TYPE, ...) auv::lua::new_sol_type_impl<TYPE>(TABLE, auv::lua::type_name_without_namespace(#TYPE), ##__VA_ARGS__)

void setup_env(sol::state &state);

extern void (*setup_env_all)(sol::state &);

}// namespace auv::lua

#endif//AUV_HOST_RUNTIME_LUA_INTEROP_H
