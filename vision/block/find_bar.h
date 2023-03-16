//
// Created by qff233 on 23-3-15.
//

#ifndef AUV_HOST_FIND_BAR_H
#define AUV_HOST_FIND_BAR_H

#include "block.h"

#include <opencv2/opencv.hpp>

namespace auv::vision {

struct FindBarResult {
  cv::Point short_side_cent_point;
  double long_side_rot;
};

class FindBarBlock : public auv::Block<const cv::Mat&, std::tuple<cv::Mat, std::vector<FindBarResult>>> {
public:
  explicit FindBarBlock(bool draw_contours = false);
  Out process(In frame) override;
  AUV_BLOCK;
private:
  bool m_draw_contours = false;
};

}

#endif//AUV_HOST_FIND_BAR_H
