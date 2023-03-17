//
// Created by qff233 on 23-3-17.
//

#ifndef AUV_HOST_OPERATOR_ROV_BLOCK_H
#define AUV_HOST_OPERATOR_ROV_BLOCK_H

#include "block.h"
#include "connect_rov.h"

namespace auv {

class OperatorRovBlock : auv::Block<std::tuple<float, float, float, float, bool>, std::tuple<>> {
public:
  explicit OperatorRovBlock(ConnectROV& rov, bool is_absolute);

  Out process(In op) override;
private:
  bool m_is_absolute;
  ConnectROV& m_rov;
};

}

#endif//AUV_HOST_OPERATOR_ROV_BLOCK_H
