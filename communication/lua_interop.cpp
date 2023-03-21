#include "lua_interop.h"
#include "runtime/lua_interop.h"

#include "rov_control_block.h"

void auv::communication::lua::setup_env(sol::state &state) {
  if (state["communication"].is<sol::table>()) return;
  auv::lua::setup_env(state);
  state.create_named_table("communication");

  {
    using RovController = auv::RovController&;
    AUV_NEW_SOL_TYPE(state, RovController,
                     sol::constructors<auv::RovController(const std::string &, int)>(),
                     "catcher", &auv::RovController::catcher,
                     "move", &auv::RovController::move,
                     "move_absolute", &auv::RovController::move_absolute,
                     "set_direction_locked", &auv::RovController::set_direction_locked,
                     "set_depth_locked", &auv::RovController::set_depth_locked);
  }

  AUV_NEW_SOL_TYPE(state, auv::RovControlBlock, sol::constructors<auv::RovControlBlock(auv::RovController &)>());

  static bool initial_invocation = true;
  if (initial_invocation) {
    auv::lua::setup_env_all = auv::communication::lua::setup_env;
    initial_invocation = false;
  }
}
