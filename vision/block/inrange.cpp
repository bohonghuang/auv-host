//
// Created by qff233 on 23-3-15.
//


#include "inrange.h"

namespace auv::vision {

InRangeBlock::InRangeBlock(ColorType type, int param1_low, int param2_low, int param3_low, int param1_high, int param2_high, int param3_high)
    : m_type(type), m_param1_low(param1_low), m_param2_low(param2_low), m_param3_low(param3_low), m_param1_high(param1_high),
      m_param2_high(param2_high), m_param3_high(param3_high) {}

void InRangeBlock::set_range(int param1_low, int param2_low, int param3_low, int param1_high, int param2_high, int param3_high) {
  m_param1_low = param1_low;
  m_param2_low = param2_low;
  m_param3_low = param3_low;
  m_param1_high = param1_high;
  m_param2_high = param2_high;
  m_param3_high = param3_high;
}

cv::Mat InRangeBlock::process(const cv::Mat &frame) {
  if (frame.channels() != 3) {
    std::cout << "The frame need three channels!" << std::endl;
    return {};
  }

  cv::Mat result;
  cv::cvtColor(frame, result, static_cast<int>(m_type));
  cv::inRange(result, cv::Scalar(m_param1_low, m_param2_low, m_param3_low),
              cv::Scalar(m_param1_high, m_param2_high, m_param3_high), result);
  return result;
}


}// namespace auv::vision
