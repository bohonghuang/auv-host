#include "lua_interop.h"
#include "block.h"
#include "concurrent.h"
#include "lua_block.h"

void (*auv::lua::setup_env_all)(sol::state &);

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
  return "";
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
  sol::table ns_char = state.create_named_table("char");
  ns_char["to_any"] = sol_object_any<char>;
  ns_char["from_any"] = any_sol_object<char>;
  sol::table ns_uchar = state.create_named_table("uchar");
  ns_uchar["to_any"] = sol_object_any<unsigned char>;
  ns_uchar["from_any"] = any_sol_object<unsigned char>;
  sol::table ns_int = state.create_named_table("int");
  ns_int["to_any"] = sol_object_any<int>;
  ns_int["from_any"] = any_sol_object<int>;
  sol::table ns_uint = state.create_named_table("uint");
  ns_uint["to_any"] = sol_object_any<unsigned int>;
  ns_uint["from_any"] = any_sol_object<unsigned int>;
  sol::table ns_short = state.create_named_table("short");
  ns_short["to_any"] = sol_object_any<short>;
  ns_short["from_any"] = any_sol_object<short>;
  sol::table ns_ushort = state.create_named_table("ushort");
  ns_ushort["to_any"] = sol_object_any<unsigned short>;
  ns_ushort["from_any"] = any_sol_object<unsigned short>;
  sol::table ns_long = state.create_named_table("long");
  ns_long["to_any"] = sol_object_any<long>;
  ns_long["from_any"] = any_sol_object<long>;
  sol::table ns_ulong = state.create_named_table("ulong");
  ns_ulong["to_any"] = sol_object_any<unsigned long>;
  ns_ulong["from_any"] = any_sol_object<unsigned long>;
  sol::table ns_longlong = state.create_named_table("longlong");
  ns_longlong["to_any"] = sol_object_any<long long>;
  ns_longlong["from_any"] = any_sol_object<long long>;
  sol::table ns_ulonglong = state.create_named_table("ulonglong");
  ns_ulonglong["to_any"] = sol_object_any<unsigned long long>;
  ns_ulonglong["from_any"] = any_sol_object<unsigned long long>;
  sol::table ns_u8 = state.create_named_table("u8");
  ns_u8["to_any"] = sol_object_any<uint8_t>;
  ns_u8["from_any"] = any_sol_object<uint8_t>;
  sol::table ns_u16 = state.create_named_table("u16");
  ns_u16["to_any"] = sol_object_any<uint16_t>;
  ns_u16["from_any"] = any_sol_object<uint16_t>;
  sol::table ns_u32 = state.create_named_table("u32");
  ns_u32["to_any"] = sol_object_any<uint32_t>;
  ns_u32["from_any"] = any_sol_object<uint32_t>;
  sol::table ns_u64 = state.create_named_table("u64");
  ns_u64["to_any"] = sol_object_any<uint64_t>;
  ns_u64["from_any"] = any_sol_object<uint64_t>;
  sol::table ns_i8 = state.create_named_table("i8");
  ns_i8["to_any"] = sol_object_any<int8_t>;
  ns_i8["from_any"] = any_sol_object<int8_t>;
  sol::table ns_i16 = state.create_named_table("i16");
  ns_i16["to_any"] = sol_object_any<int16_t>;
  ns_i16["from_any"] = any_sol_object<int16_t>;
  sol::table ns_i32 = state.create_named_table("i32");
  ns_i32["to_any"] = sol_object_any<int32_t>;
  ns_i32["from_any"] = any_sol_object<int32_t>;
  sol::table ns_i64 = state.create_named_table("i64");
  ns_i64["to_any"] = sol_object_any<int64_t>;
  ns_i64["from_any"] = any_sol_object<int64_t>;
  sol::table ns_float = state.create_named_table("float");
  sol::table ns_double = state.create_named_table("double");
  sol::table ns_f32 = state.create_named_table("f32");
  sol::table ns_f64 = state.create_named_table("f64");
  ns_float["to_any"] = ns_f32["to_any"] = sol_object_any<float>;
  ns_double["to_any"] = ns_f64["to_any"] = sol_object_any<double>;
  ns_float["from_any"] = ns_f32["from_any"] = any_sol_object<float>;
  ns_double["from_any"] = ns_f64["from_any"] = any_sol_object<double>;
  AUV_NEW_SOL_TYPE(state, std::any, sol::default_constructor,
                   "has_value", &std::any::has_value,
                   "reset", &std::any::reset);
  state["any"]["from"] = [](sol::object obj) -> std::any {
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
        return obj.as<sol::userdata>()["to_any"].call<std::any>(obj);
      case sol::type::table:
        return obj.as<sol::table>()["to_any"].call<std::any>(obj);
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
  sol::table ns_runtime = state.create_named_table("runtime");
  state.new_usertype<auv::unit_t>("unit", sol::default_constructor,
                                  "to_any", [](auv::unit_t in) -> std::any { return in; });
  state["void"] = unit_t{};
  register_conversion(state);
  state.new_usertype<sol::state>(
      "LuaState", sol::default_constructor,
      "script", [](sol::state &state, const std::string &script) { return state.script(script); },
      "script_file", [](sol::state &state, const std::string &filename) { return state.script_file(filename); });
  static constexpr auto to_any_block = [](const sol::userdata &&block) -> auv::AnyBlock {
    return block.is<auv::AnyBlock>() ? block.as<auv::AnyBlock>() : block["as_untyped"].call<auv::AnyBlock>(block);
  };
  static constexpr auto tee_block = [](sol::variadic_args va) -> auv::AnyBlock {
    auto &&begin = *va.cbegin();
    auto prev_block = to_any_block(begin);
    for (auto iter = va.cbegin() + 1; iter != va.cend(); iter++) {
      prev_block = TeeBlock<AnyBlock, AnyBlock>{prev_block, to_any_block(*iter)}.as_untyped();
    }
    return prev_block;
  };
  static constexpr auto chain_block = [](sol::variadic_args va) -> auv::AnyBlock {
    auto &&begin = *va.cbegin();
    auto prev_block = to_any_block(begin);
    for (auto iter = va.cbegin() + 1; iter != va.cend(); iter++) {
      prev_block = prev_block.connect(to_any_block(*iter));
    }
    return prev_block;
  };
  AUV_NEW_SOL_TYPE(
      state, auv::AnyBlock, sol::no_constructor,
      "connect", [](auv::AnyBlock &self, sol::variadic_args va) -> auv::AnyBlock {
        return self.connect(tee_block(va));
      },
      AUV_BLOCK_SOL_METHODS(auv::AnyBlock));
  state["tee"] = tee_block;
  state["chain"] = state["connect"] = chain_block;
  using LuaBlock = UntypedLuaBlock;
  AUV_NEW_SOL_TYPE(state, LuaBlock,
                   sol::factories(
                       []() -> UntypedLuaBlock {
                         UntypedLuaBlock block{};
                         return block;
                       },
                       [](const std::string &filename) -> UntypedLuaBlock {
                         UntypedLuaBlock block{};
                         block.lua().script_file(filename);
                         return block;
                       },
                       [](sol::function fun) -> UntypedLuaBlock {
                         UntypedLuaBlock block{};
                         block.lua()["process"] = [=](std::any in) -> std::any {
                           return fun.call<std::any>(in);
                         };
                         return block;
                       }),
                   "lua", &UntypedLuaBlock::lua,
                   AUV_BLOCK_SOL_METHODS(UntypedLuaBlock));
  AUV_NEW_SOL_TYPE(state, UntypedMuxBlock::InputBlock, sol::no_constructor,
                   AUV_BLOCK_SOL_METHODS(UntypedMuxBlock::InputBlock));

  using LuaMuxBlock = UntypedLuaMuxBlock;
  AUV_NEW_SOL_TYPE(state, LuaMuxBlock,
                   sol::factories(
                       []() -> UntypedLuaMuxBlock {
                         UntypedLuaMuxBlock block {};
                         return block;
                       },
                       [](std::string filename) -> UntypedLuaMuxBlock {
                         UntypedLuaMuxBlock block {};
                         block.lua().script_file(filename);
                         return block;
                       },
                       [](sol::function fun) -> UntypedLuaMuxBlock {
                         UntypedLuaMuxBlock block {};
                         block.lua()["process"] = [=](std::unordered_map<std::string, std::any> &in) -> std::any {
                           return fun.call<std::any>(in);
                         };
                         return block;
                       }),
                   "lua", &UntypedLuaMuxBlock::lua,
                   "input_block", &UntypedLuaMuxBlock::input_block,
                   AUV_BLOCK_SOL_METHODS(UntypedLuaMuxBlock));

  state.new_usertype<Scheduler>("Scheduler",
                                sol::factories([](AnyBlock block, float secs) { return std::make_shared<Scheduler>(block, std::chrono::milliseconds(static_cast<int>(secs * 1000.0f))); }),
                                "start", &Scheduler::start,
                                "stop", &Scheduler::stop,
                                "pause", &Scheduler::pause,
                                "resume", &Scheduler::resume);
  state["Scheduler"]["from_any"] = object_from_any_function<std::shared_ptr<Scheduler>>;
  state["Scheduler"]["to_any"] = object_to_any_function<std::shared_ptr<Scheduler>>;

  state.set_function("sleep", [](float sec) -> void {
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sec * 1000)));
  });

  state.set_function("current_time", []() -> float {
    return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / 1000.0f;
  });

  static bool initial_invocation = true;
  if (initial_invocation) {
    auv::lua::setup_env_all = auv::lua::setup_env;
    initial_invocation = false;
  }
}
