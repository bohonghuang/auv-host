//
// Created by qff233 on 23-3-17.
//

#include "imshow.h"

namespace auv::vision {


std::tuple<> ImshowBlock::process(cv::Mat frame) noexcept {
  cv::imshow("preview", frame);
  cv::waitKey(1);
  return {};
}

}
