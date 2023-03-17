//
// Created by qff233 on 23-3-15.
//

#ifndef AUV_HOST_COLOR_H
#define AUV_HOST_COLOR_H

#include <opencv2/opencv.hpp>

#include "block.h"

namespace auv::vision {

class ConvertColorBlock : public auv::Block<cv::Mat, cv::Mat> {
public:
  explicit ConvertColorBlock(int code);
  Out process(In in) override;
  AUV_BLOCK;
private:
  int m_code;
};

struct InRangeParams {
  int low_1;
  int high_1;
  int low_2;
  int high_2;
  int low_3;
  int high_3;
};

class InRangeBlock : public auv::Block<cv::Mat, cv::Mat> {
public:
  InRangeBlock() = default;
  explicit InRangeBlock(const InRangeParams& params);
  void set_params(const InRangeParams& params);
  void set_range(int param1_low, int param2_low, int param3_low, int param1_high, int param2_high, int param3_high);
  Out process(In) override;
  AUV_BLOCK;
private:
  InRangeParams m_params{};
};

}// namespace auv::vision

#endif//AUV_HOST_COLOR_H
