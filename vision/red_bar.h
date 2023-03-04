//
// Created by Qff on 2023/3/4.
//

#ifndef RED_BAR_H
#define RED_BAR_H

#include "block.h"

namespace auv::vision {

class RedBarBlock : public ThresholdBlock<int> {
public:
  RedBarBlock();
  AlgorithmResult process_imp(const cv::Mat &frame, auv::vision::TimeStep ts) noexcept override;
};

}// namespace auv::vision

#endif//RED_BAR_H
