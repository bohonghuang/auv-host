#include "detect.h"

namespace auv::vision {

ObjectDetectBlock::ObjectDetectBlock(const std::string &model_data)
    : m_yolo(model_data) {}

ObjectDetectBlock::Out ObjectDetectBlock::process(cv::Mat frame) {
  auto map_result = m_yolo.process(frame, m_yolo.forward(frame));
  std::vector<network::YoloFastV2Result> result;
  for (auto it = map_result.begin(); it != map_result.end(); ++it) {
    for (const auto& i : it->second) {
      result.push_back(i);
    }
  }
  return {frame, result};
}

}// namespace auv::vision
