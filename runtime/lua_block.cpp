#include "lua_block.h"

std::any auv::UntypedLuaMuxBlock::process(auv::unit_t in) {
  return (*m_state)["process"].call<std::any>(m_buffer);
}

auv::UntypedLuaMuxBlock::UntypedLuaMuxBlock() {
  auv::lua::setup_env(*m_state);
}

auv::UntypedLuaBlock::UntypedLuaBlock() {
  auv::lua::setup_env(*m_state);
}
