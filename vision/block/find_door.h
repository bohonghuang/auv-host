#ifndef AUV_HOST_FIND_DOOR_H
#define AUV_HOST_FIND_DOOR_H

#include "block.h"

#include <opencv2/core/matx.hpp>
#include <opencv2/opencv.hpp>

namespace auv::vision {

struct FindDoorResult {
  std::array<float, 4> left;
  std::array<float, 4> right;
  std::array<float, 4> bottom;
};

struct FindDoorResults {
  cv::Mat frame;
  FindDoorResult result;
};

class FindLineBlock : public auv::Block<cv::Mat, FindDoorResults> {
public:
  FindLineBlock(double rho, double theta, int threshold, bool center_div = true);
  Out process(In frame) override;
  AUV_BLOCK;

private:
  double m_rho;
  double m_theta;
  int m_threshold;

  bool m_center_div;
};

}// namespace auv::vision

#endif//AUV_HOST_FIND_DOOR_H
