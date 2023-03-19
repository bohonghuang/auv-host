#include <catch2/catch_test_macros.hpp>

#include <sol/sol.hpp>

#include "lua_block.h"
#include "lua_interop.h"
#include "test.h"

namespace auv::runtime::test {

TEST_CASE("C++ 中的静态 Block 连接") {
  Plus1Block plus1;
  Times2Block times2;
  auto block = plus1 | times2 | plus1 | times2;
  auto test = [&](int i) {
    REQUIRE(block(i) == ((i + 1) * 2 + 1) * 2);
  };
  for (int i = 0; i < 10; i++) test(i);
}

TEST_CASE("Block 的类型擦除") {
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

TEST_CASE("Lua 中 `std::any` 的类型转换与 Block 的动态连接") {
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

TEST_CASE("LuaBlock") {
  sol::state state;
  setup_env(state);
  SECTION("创建 LuaBlock 中的虚拟机与定义处理函数") {
    state.script(R"?(
block = LuaBlock.new()
state = block:lua()
state:script("function process(x) return int.to_any(int.from_any(x) + 1) end")
)?");
    sol::state &state2 = state["state"];
    REQUIRE(state2["process"].is<sol::function>());
    UntypedLuaBlock &block = state["block"];
    REQUIRE(std::any_cast<int>(block(1)) == 2);
  }
  SECTION("传入函数作为 LuaBlock 的处理函数") {
    state.script(R"?(
block = LuaBlock.new(function(x) return int.to_any(int.from_any(x) + 1) end)
  )?");
    REQUIRE(state["block"].is<UntypedLuaBlock>());
    UntypedLuaBlock &block = state["block"];
    REQUIRE(std::any_cast<int>(block(1)) == 2);
  }
}

TEST_CASE("LuaMuxBlock") {
  sol::state state;
  setup_env(state);
  SECTION("创建 LuaMuxBlock 中的虚拟机与定义处理函数") {
    state.script(R"?(
block = LuaMuxBlock.new()
state = block:lua()
state:script("function process(x) return int.to_any(int.from_any(x[\"i\"]) + 1) end")
)?");
    sol::state &state2 = state["state"];
    REQUIRE(state2["process"].is<sol::function>());
    UntypedLuaMuxBlock &block = state["block"];
    block.input_block("i")(1);
    REQUIRE(std::any_cast<int>(block({})) == 2);
  }
  SECTION("与 LuaBlock 连接") {
    state.script(R"?(
mux_block = LuaMuxBlock.new(function (x) return int.to_any(int.from_any(x["i"]) * int.from_any(x["j"]) + 1) end)
block_i = LuaBlock.new(function (x) return int.to_any(3) end)
block_j = LuaBlock.new(function (x) return int.to_any(2) end)
connect(block_i, mux_block:input_block("i")):process(void:as_untyped())
connect(block_j, mux_block:input_block("j")):process(void:as_untyped())
result = int.from_any(mux_block:process(void))
    )?");
    REQUIRE(state.get<int>("result") == 7);
  }
}


}// namespace auv::runtime::test
