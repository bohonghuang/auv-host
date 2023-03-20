#include "lua_block.h"

std::any auv::UntypedLuaMuxBlock::process(auv::unit_t in) {
  decltype(this->m_block->m_buffer) buffer;
  {
    std::lock_guard<std::mutex> lock_guard { this->m_block->m_buffer_mutex };
    buffer = m_block->m_buffer;
  }
  return (*m_state)["process"].call<std::any>(std::move(buffer));
}

auv::UntypedLuaMuxBlock::UntypedLuaMuxBlock() {
  auv::lua::setup_env_all(*m_state);
}

auv::UntypedLuaBlock::UntypedLuaBlock() {
  auv::lua::setup_env_all(*m_state);
}
