#include <catch2/catch_test_macros.hpp>

#include <sol/sol.hpp>

#include "lua_interop.h"
#include "test.h"

namespace auv::runtime::test {

TEST_CASE("C++ 中的静态块连接") {
  Plus1Block plus1;
  Times2Block times2;
  auto block = plus1 | times2 | plus1 | times2;
  auto test = [&](int i) {
    REQUIRE(block(i) == ((i + 1) * 2 + 1) * 2);
  };
  for (int i = 0; i < 10; i++) test(i);
}

TEST_CASE("块的类型擦除") {
  Plus1Block plus1;
  Times2Block times2;
  auto block = ((plus1.as_into_untyped() | times2.as_from_untyped() | plus1).as_untyped() | times2.as_untyped()).as_untyped().as_untyped().as_typed<int, int>();
  auto test = [&](int i) {
    REQUIRE(block(i) == ((i + 1) * 2 + 1) * 2);
  };
  for (int i = 0; i < 10; i++) test(i);
}

void setup_env(sol::state &state) {
  auv::lua::setup_env(state);
  state.new_usertype<Plus1Block>("Plus1Block", sol::default_constructor,
                                 AUV_BLOCK_SOL_METHODS(Plus1Block));
  state.new_usertype<Times2Block>("Times2Block", sol::default_constructor,
                                  AUV_BLOCK_SOL_METHODS(Times2Block));
}

TEST_CASE("Lua 中 `std::any` 的类型转换与块的动态连接") {
  sol::state state;
  setup_env(state);
  state.script(R"(
function test(i)
  plus1 = Plus1Block.new()
  times2 = Times2Block.new()
  block = connect(plus1, times2, plus1, times2)
  return int.from_any(block:process(int.to_any(i)))
end
)");
  auto test = [&](int i) {
    REQUIRE(state["test"].call<int>(i) == ((i + 1) * 2 + 1) * 2);
  };
  for (int i = 0; i < 10; i++) test(i);
}

}// namespace auv::runtime::test
