#include "lua_block.h"

std::any auv::UntypedLuaMuxBlock::process(auv::unit_t in) {
  return (*m_state)["process"].call<std::any>(m_buffer);
}
