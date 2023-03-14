//
// Created by Qff on 2023/3/5.
//

#ifndef RED_DOOR_H
#define RED_DOOR_H

#include "block.h"

namespace auv::vision {

class RedDoorBlock : public ThresholdBlock<int> {
public:
  RedDoorBlock(Camera &camera);

protected:
  AlgorithmResult process_imp(const cv::Mat &frame, auv::vision::TimeStep ts) noexcept override;
};

}// namespace auv::vision

#endif//RED_DOOR_H