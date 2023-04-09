#ifndef AUV_HOST_FIND_DOOR_H
#define AUV_HOST_FIND_DOOR_H

#include "block.h"

#include <opencv2/opencv.hpp>

namespace auv::vision {

struct FindDoorResults {
  cv::Mat frame;
  std::vector<cv::Rect2f> result;
};

class FindLineBlock : public auv::Block<cv::Mat, FindDoorResults> {
public:
  FindLineBlock(double rho, double theta, int threshold);
  Out process(In frame) override;
  AUV_BLOCK;

private:
  double m_rho;
  double m_theta;
  int m_threshold;
};

}// namespace auv::vision

#endif//AUV_HOST_FIND_DOOR_H
