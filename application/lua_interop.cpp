#include "lua_interop.h"
#include "lua_block.h"

void auv::application::lua::setup_env(sol::state &state) {
  if(state["application"].is<sol::table>()) return;
  state.create_named_table("application");
  auv::lua::setup_env(state);
  auv::communication::lua::setup_env(state);
  auv::vision::lua::setup_env(state);
  state["LuaBlock"]["new"] = [](const std::string& filename) -> auv::UntypedLuaBlock {
    auv::UntypedLuaBlock block {};
    setup_env(block.lua());
    block.lua().script_file(filename);
    return block;
  };
  state["LuaMuxBlock"]["new"] = [](const std::string& filename) -> auv::UntypedLuaMuxBlock {
    auv::UntypedLuaMuxBlock block {};
    setup_env(block.lua());
    block.lua().script_file(filename);
    return block;
  };
}
