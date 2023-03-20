#include "detect.h"

namespace auv::vision {

ObjectDetectBlock::ObjectDetectBlock(const std::string &model_data)
    : m_yolo(model_data) {}

ObjectDetectBlock::Out ObjectDetectBlock::process(cv::Mat frame) {
  return {frame, m_yolo.process(frame, m_yolo.forward(frame))};
}

}// namespace auv::vision
