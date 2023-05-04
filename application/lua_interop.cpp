#include "lua_interop.h"
#include "runtime/lua_interop.h"
#include "vision/lua_interop.h"

void auv::application::lua::setup_env(sol::state &state) {
  if (state["application"].is<sol::table>()) return;
  state.create_named_table("application");
  auv::lua::setup_env(state);
  auv::vision::lua::setup_env(state);
  static bool initial_invocation = true;
  if (initial_invocation) {
    auv::lua::setup_env_all = auv::application::lua::setup_env;
    initial_invocation = false;
  }
}
