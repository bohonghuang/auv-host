#include "lua_interop.h"
#include "block.h"
#include "lua_block.h"

const char *sol_type_name(sol::type tpe) {
  switch (tpe) {
    case sol::type::none:
      return "sol::type::none";
    case sol::type::lua_nil:
      return "sol::type::lua_nil";
    case sol::type::string:
      return "sol::type::string";
    case sol::type::number:
      return "sol::type::number";
    case sol::type::thread:
      return "sol::type::thread";
    case sol::type::boolean:
      return "sol::type::boolean";
    case sol::type::function:
      return "sol::type::function";
    case sol::type::userdata:
      return "sol::type::userdata";
    case sol::type::lightuserdata:
      return "sol::type::lightuserdata";
    case sol::type::table:
      return "sol::type::table";
    case sol::type::poly:
      return "sol::type::poly";
  }
};

template<sol::type tpe>
struct sol_type_cpp_type_impl {};

template<>
struct sol_type_cpp_type_impl<sol::type::number> {
  using type = double;
};

template<sol::type tpe>
using sol_type_cpp_type = typename sol_type_cpp_type_impl<tpe>::type;

template<class T>
struct cpp_type_sol_type_impl {
  constexpr static sol::type type = std::conditional_t < std::is_integral_v<T> || std::is_floating_point_v<T>, std::integral_constant<sol::type, sol::type::number>, std::false_type > ();
};

template<sol::type tpe>
using sol_type_cpp_type = typename sol_type_cpp_type_impl<tpe>::type;

template<class tpe>
static constexpr sol::type cpp_type_sol_type = cpp_type_sol_type_impl<tpe>::type;

template<class cpp_result_type, sol::type sol_type = cpp_type_sol_type<cpp_result_type>, class cpp_receive_type = sol_type_cpp_type<sol_type>>
std::any sol_object_any(sol::object obj) {
  if (obj.get_type() == sol_type) {
    return static_cast<cpp_result_type>(obj.as<cpp_receive_type>());
  } else {
    std::stringstream err;
    err << "Cannot convert Lua type `" << sol_type_name(obj.get_type()) << "` to C++ type `" << typeid(cpp_receive_type).name() << "`.";
    throw std::runtime_error(err.str());
  }
};

template<class cpp_type, sol::type sol_type = cpp_type_sol_type<cpp_type>>
cpp_type any_sol_object(sol::object obj) {
  if (obj.is<std::any>()) {
    return std::any_cast<cpp_type>(obj.as<std::any>());
  } else {
    std::stringstream err;
    err << "The Lua object `" << sol_type_name(sol_type) << "` is not `" << typeid(std::any).name() << "`.";
    throw std::runtime_error(err.str());
  }
};

void register_conversion(sol::state &state) {
  state["char_any"] = sol_object_any<char>;
  state["any_char"] = any_sol_object<char>;
  state["uchar_any"] = sol_object_any<unsigned char>;
  state["any_uchar"] = any_sol_object<unsigned char>;
  state["int_any"] = sol_object_any<int>;
  state["any_int"] = any_sol_object<int>;
  state["uint_any"] = sol_object_any<unsigned int>;
  state["any_uint"] = any_sol_object<unsigned int>;
  state["short_any"] = sol_object_any<short>;
  state["any_short"] = any_sol_object<short>;
  state["ushort_any"] = sol_object_any<unsigned short>;
  state["any_ushort"] = any_sol_object<unsigned short>;
  state["long_any"] = sol_object_any<long>;
  state["any_long"] = any_sol_object<long>;
  state["ulong_any"] = sol_object_any<unsigned long>;
  state["any_ulong"] = any_sol_object<unsigned long>;
  state["longlong_any"] = sol_object_any<long long>;
  state["any_longlong"] = any_sol_object<long long>;
  state["ulonglong_any"] = sol_object_any<unsigned long long>;
  state["any_ulonglong"] = any_sol_object<unsigned long long>;
  state["u8_any"] = sol_object_any<uint8_t>;
  state["any_u8"] = any_sol_object<uint8_t>;
  state["u16_any"] = sol_object_any<uint16_t>;
  state["any_u16"] = any_sol_object<uint16_t>;
  state["u32_any"] = sol_object_any<uint32_t>;
  state["any_u32"] = any_sol_object<uint32_t>;
  state["u64_any"] = sol_object_any<uint64_t>;
  state["any_u64"] = any_sol_object<uint64_t>;
  state["i8_any"] = sol_object_any<int8_t>;
  state["any_i8"] = any_sol_object<int8_t>;
  state["i16_any"] = sol_object_any<int16_t>;
  state["any_i16"] = any_sol_object<int16_t>;
  state["i32_any"] = sol_object_any<int32_t>;
  state["any_i32"] = any_sol_object<int32_t>;
  state["i64_any"] = sol_object_any<int64_t>;
  state["any_i64"] = any_sol_object<int64_t>;
  state["f32_any"] = state["float_any"] = sol_object_any<float>;
  state["f64_any"] = state["double_any"] = sol_object_any<double>;
  state["any_f32"] = state["any_float"] = any_sol_object<float>;
  state["any_f64"] = state["any_double"] = any_sol_object<double>;
  state["any"] = [](sol::object obj) -> std::any {
    const char *type_name;
    switch (obj.get_type()) {
      case sol::type::none:
      case sol::type::lua_nil:
        return auv::unit_t{};
      case sol::type::string:
        return obj.as<std::string>();
      case sol::type::number:
        return obj.as<double>();
      case sol::type::boolean:
        return obj.as<bool>();
      case sol::type::userdata:
        return obj.as<sol::userdata>()["as_untyped"].call<std::any>(obj);
      case sol::type::table:
        return obj.as<sol::table>()["as_untyped"].call<std::any>(obj);
      default:
        type_name = sol_type_name(obj.get_type());
        break;
    }
    std::stringstream err;
    err << "Cannot convert Lua type `" << type_name << "` to `std::any`.";
    throw std::runtime_error(err.str());
  };
}

void auv::lua::setup_env(sol::state &state) {
  if (state["runtime"].is<sol::table>()) return;
  state.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::jit);
  state.create_named_table("runtime");
  register_conversion(state);
  state.new_usertype<auv::AnyBlock>(
      "AnyBlock", sol::no_constructor,
      "process", sol::overload(sol::resolve<std::any(std::any)>(&auv::AnyBlock::process), sol::resolve<std::any()>(&auv::AnyBlock::process)),
      "connect", &auv::AnyBlock::connect);

  state.set_function("connect", [](sol::variadic_args va) -> auv::AnyBlock {
    auto &&begin = *va.cbegin();
    static constexpr auto to_any_block = [](const sol::userdata &&block) -> auv::AnyBlock {
      return block.is<auv::AnyBlock>() ? block.as<auv::AnyBlock>() : block["as_untyped"].call<auv::AnyBlock>(block);
    };
    auto prev_block = to_any_block(begin);
    for (auto iter = va.cbegin() + 1; iter != va.cend(); iter++) {
      prev_block = prev_block.connect(to_any_block(*iter));
    }
    return prev_block;
  });
  state.new_usertype<UntypedLuaBlock>("LuaBlock",
                                      sol::no_constructor,
                                      AUV_BLOCK_SOL_METHODS(auv::UntypedLuaBlock));
  state.new_usertype<UntypedLuaMuxBlock>("LuaMuxBlock",
                                         sol::no_constructor,
                                         AUV_BLOCK_SOL_METHODS(auv::UntypedLuaMuxBlock));
}
