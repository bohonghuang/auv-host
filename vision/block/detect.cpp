#include "detect.h"

namespace auv::vision {

ObjectDetectBlock::ObjectDetectBlock(const std::string &model_data)
    : m_yolo(model_data) {}

ObjectDetectBlock::Out
ObjectDetectBlock::process(cv::Mat frame) {
  auto map_result = m_yolo.process(frame, m_yolo.forward(frame));
  std::vector<ObjectDetectResult> result;
  for (auto it = map_result.begin(); it != map_result.end(); ++it) {
    for (const auto &i: it->second) {
      static double normalization_x = 2.0 / frame.size().width;
      static double normalization_y = 2.0 / frame.size().height;
      double x = i.rect.x * normalization_x - 1.0;
      double y = -(i.rect.y * normalization_y - 1.0);
      double width = i.rect.width * normalization_x;
      double height = i.rect.height * normalization_y;
      result.push_back({i.name, i.confidence, x, y, width, height});

      network::YoloFastV2::draw_pred(frame, i.confidence, i.name, i.rect);
    }
  }

  return {frame, result};
}

}// namespace auv::vision
