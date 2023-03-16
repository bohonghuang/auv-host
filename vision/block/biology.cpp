//
// Created by qff233 on 23-3-16.
//

#include "biology.h"

namespace auv::vision {

FindBiologyBlock::FindBiologyBlock(const std::string &data_path)
    : m_yolo(data_path) {}

FindBiologyBlock::Out FindBiologyBlock::process(const cv::Mat& frame) {
  return m_yolo.process(frame, m_yolo.forward(frame));
}

}// namespace auv::vision