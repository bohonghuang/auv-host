#include "biology.h"

namespace auv::vision {

FindBiologyBlock::FindBiologyBlock(const std::string &model_data)
    : m_yolo(model_data) {}

FindBiologyBlock::Out FindBiologyBlock::process(cv::Mat frame) {
  return {frame, m_yolo.process(frame, m_yolo.forward(frame))};
}

}// namespace auv::vision
