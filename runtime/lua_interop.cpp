#include "lua_interop.h"
#include "block.h"

void auv::lua::setup_env(sol::state &state) {
  if (state["runtime"].is<sol::table>()) return;
  state.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::jit);
  state.create_named_table("runtime");
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
}
