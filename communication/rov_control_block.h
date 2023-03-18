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

class RovControlBlock : public auv::Block<std::function<void(std::shared_ptr<RovController>)>, auv::unit_t> {
public:
  explicit RovControlBlock(std::shared_ptr<RovController> rov);
  Out process(In) noexcept override;
  AUV_BLOCK;

private:
  std::shared_ptr<RovController> m_rov;
};

}// namespace auv

#endif//AUV_HOST_ROV_CONTROL_BLOCK_H
