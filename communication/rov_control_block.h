#ifndef AUV_HOST_ROV_CONTROL_BLOCK_H
#define AUV_HOST_ROV_CONTROL_BLOCK_H

#include <functional>

#include "block.h"
#include "rov_controller.h"


namespace auv {

struct OperatorMotion {
  float x;
  float y;
  float z;
};

class RovControlBlock : public auv::Block<std::function<void(RovController&)>, auv::unit_t> {
public:
  explicit RovControlBlock(RovController& rov);
  Out process(In) noexcept override;
  AUV_BLOCK;

private:
  RovController& m_rov;
};

}// namespace auv

#endif//AUV_HOST_ROV_CONTROL_BLOCK_H
