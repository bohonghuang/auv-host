#include "color.h"

namespace auv::vision {

ConvertColorBlock::ConvertColorBlock(int code) : m_code(code) {}

cv::Mat
ConvertColorBlock::process(cv::Mat input) {
  cv::Mat output;
  cv::cvtColor(input, output, m_code);
  return output;
}

InRangeBlock::InRangeBlock(const InRangeParams &params)
    : m_params(params) {}

void InRangeBlock::set_range(int param1_low, int param2_low, int param3_low, int param1_high, int param2_high, int param3_high) {
  m_params.low_1 = param1_low;
  m_params.low_2 = param2_low;
  m_params.low_3 = param3_low;
  m_params.high_1 = param1_high;
  m_params.high_2 = param2_high;
  m_params.high_3 = param3_high;
}

void InRangeBlock::set_params(const InRangeParams &params) {
  m_params = params;
}

cv::Mat
InRangeBlock::process(cv::Mat frame) {
  cv::Mat result;
  cv::inRange(frame, cv::Scalar(m_params.low_1, m_params.low_2, m_params.low_3),
              cv::Scalar(m_params.high_1, m_params.high_2, m_params.high_3), result);
  return result;
}


}// namespace auv::vision
