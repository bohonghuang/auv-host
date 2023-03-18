#include "biology.h"

namespace auv::vision {

FindBiologyBlock::FindBiologyBlock(const std::string &model_name)
    : m_yolo(model_name) {}

FindBiologyBlock::Out FindBiologyBlock::process(cv::Mat frame) {
  return m_yolo.process(frame, m_yolo.forward(frame));
}

}// namespace auv::vision
