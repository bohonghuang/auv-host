#ifndef AUV_HOST_FIND_LINE_H
#define AUV_HOST_FIND_LINE_H

#include "block.h"

#include <opencv2/opencv.hpp>

namespace auv::vision {

struct FindLineResult {
  double theta;
  cv::Point2d point;
};

struct FindLineResults {
  cv::Mat frame;
  std::vector<FindLineResult> result;
};

class FindLineBlock : public auv::Block<cv::Mat, FindLineResults> {
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

#endif//AUV_HOST_FIND_LINE_H
