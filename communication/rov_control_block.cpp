#include "cpp-httplib/httplib.h"
#include "jsonrpccxx/client.hpp"

#include "rov_control_block.h"

#include <utility>

namespace auv {

RovControlBlock::RovControlBlock(RovController& rov)
    : m_rov(rov) {}


auv::unit_t RovControlBlock::process(std::function<void(RovController&)> send) noexcept {
  send(m_rov);
  return {};
}

//auv::unit_t RovControlBlock::process(std::tuple<float, float, float, float, bool> op) noexcept {
//  auto [x, y, z, rot, catcher] = op;
//  if(m_is_absolute)
//    m_rov.move_absolute(x, y, z, rot);
//  else
//    m_rov.move(x, y, z, rot);
//  m_rov.catcher(catcher);
//  return {};
//}


}// namespace auv
