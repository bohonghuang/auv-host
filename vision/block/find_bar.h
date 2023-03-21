#ifndef AUV_HOST_FIND_BAR_H
#define AUV_HOST_FIND_BAR_H

#include "block.h"

#include <opencv2/opencv.hpp>

namespace auv::vision {

struct FindBarResult {
  cv::Point2d point;
  double angle;
};

struct FindBarResults {
  cv::Mat frame;
  std::vector<FindBarResult> result;
};

class FindBarBlock : public auv::Block<cv::Mat, FindBarResults> {
public:
  explicit FindBarBlock(bool debug = false);

  Out process(In frame) override;
  AUV_BLOCK;
private:
  bool m_debug = false;
};

}

#endif//AUV_HOST_FIND_BAR_H
