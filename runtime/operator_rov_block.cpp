//
// Created by qff233 on 23-3-17.
//
#include "operator_rov_block.h"

namespace auv {

OperatorRovBlock::OperatorRovBlock(ConnectROV &rov, bool is_absolute)
    : m_rov(rov), m_is_absolute(is_absolute) {}

std::tuple<> OperatorRovBlock::process(std::tuple<float, float, float, float, bool> op) {
  auto [x, y, z, rot, catcher] = op;
  if(m_is_absolute)
    m_rov.move_absolute(x, y, z, rot);
  else
    m_rov.move(x, y, z, rot);
  m_rov.catcher(catcher);
  return {};
}


}// namespace auv
