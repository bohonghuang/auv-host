#include "lua_block.h"

std::any auv::UntypedLuaMuxBlock::process(auv::unit_t in) {
  std::unordered_map<UntypedMuxBlock::Key, std::any> buffer;
  {
    auto lock_guard = m_ptr->buffer_lock_guard();
    buffer = m_ptr->buffer();
  }
  return (*m_state)["process"].call<std::any>(std::move(buffer));
}

auv::UntypedLuaMuxBlock::UntypedLuaMuxBlock(): SharedUntypedMuxBlock(std::make_shared<MuxBlock>()) {
  auv::lua::setup_env_all(*m_state);
}

auv::UntypedLuaBlock::UntypedLuaBlock() {
  auv::lua::setup_env_all(*m_state);
}
