//
// Created by qff233 on 23-3-15.
//

#ifndef AUV_HOST_INRANGE_H
#define AUV_HOST_INRANGE_H

#include <opencv2/opencv.hpp>

#include "block.h"

namespace auv::vision {

class InRangeBlock : public auv::Block<cv::Mat, cv::Mat> {
public:
  enum class ColorType {
    YCrCb = cv::COLOR_BGR2YCrCb,
    HLS = cv::COLOR_BGR2HLS,
    HSV = cv::COLOR_BGR2HSV
  };

  InRangeBlock(ColorType type, int param1_low, int param2_low, int param3_low, int param1_high, int param2_high, int param3_high);

  void set_range(int param1_low, int param2_low, int param3_low, int param1_high, int param2_high, int param3_high);

  Out process(const In&) override;

private:
  int m_param1_low;
  int m_param2_low;
  int m_param3_low;
  int m_param1_high;
  int m_param2_high;
  int m_param3_high;
  ColorType m_type;
};

}// namespace auv::vision

#endif//AUV_HOST_INRANGE_H
