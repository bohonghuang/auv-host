#include <catch2/catch_test_macros.hpp>

#include <sol/state.hpp>

#include "application/lua_interop.h"

TEST_CASE("LuaBlock 环境的初始化") {
  sol::state state;
  auv::application::lua::setup_env(state);
  REQUIRE(state["runtime"].is<sol::table>());
  REQUIRE(state["vision"].is<sol::table>());
  REQUIRE(state["application"].is<sol::table>());
  state.script(R"?(
block = LuaBlock.new()
state = block:lua()
)?");
  sol::state &state2 = state["state"];
  REQUIRE(state2["runtime"].is<sol::table>());
  REQUIRE(state2["vision"].is<sol::table>());
  REQUIRE(state2["application"].is<sol::table>());
}
