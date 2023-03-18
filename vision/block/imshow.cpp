#include "imshow.h"

namespace auv::vision {


auv::unit_t ImshowBlock::process(cv::Mat frame) noexcept {
  cv::imshow("preview", frame);
  cv::waitKey(1);
  return {};
}

}
