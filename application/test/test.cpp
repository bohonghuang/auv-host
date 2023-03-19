#include <catch2/catch_test_macros.hpp>

#include <sol/state.hpp>

#include "application/lua_interop.h"

TEST_CASE("LuaBlock 环境的自动加载") {
  sol::state state;
  auv::lua::setup_env_all(state);
  REQUIRE(state["runtime"].is<sol::table>());
  REQUIRE(state["vision"].is<sol::table>());
  REQUIRE(state["communication"].is<sol::table>());
  REQUIRE(state["application"].is<sol::table>());
}
