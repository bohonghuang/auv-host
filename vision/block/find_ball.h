//
// Created by qff233 on 23-3-22.
//

#ifndef AUV_HOST_FIND_BALL_H
#define AUV_HOST_FIND_BALL_H

#include <opencv2/opencv.hpp>

#include "block.h"

namespace auv::vision {

struct FindBallResult {
  std::array<cv::Point2d, 4> points;
};

struct FindBallResults {
  cv::Mat frame;
  std::vector<FindBallResult> result;
};

class FindBallBlock : public auv::Block<cv::Mat, FindBallResults> {
public:
  explicit FindBallBlock(bool debug = false);

  Out process(In frame) override;
  AUV_BLOCK;

private:
  bool m_debug = false;
};

}// namespace auv::vision

#endif//AUV_HOST_FIND_BALL_H