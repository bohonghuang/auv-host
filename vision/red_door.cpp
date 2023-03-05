//
// Created by Qff on 2023/3/5.
//

#include "red_door.h"
#include "utils.h"

namespace auv::vision {

RedDoorBlock::RedDoorBlock() {
  m_threshold_params = {
      {33, 177},
      {146, 255},
      {65, 130}};
}

AlgorithmResult RedDoorBlock::process_imp(const cv::Mat &frame, auv::vision::TimeStep ts) noexcept {
  AlgorithmResult result;

  cv::Mat process_frame = frame.clone();
  cv::cvtColor(process_frame, process_frame, cv::COLOR_BGR2YCrCb);
  cv::inRange(process_frame, get_low_param(), get_high_param(), process_frame);

  int high = m_frame_size.height;
  static cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                     cv::Size(high / 52, high / 52));
  static cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                     cv::Size(high / 24, high / 24));
  cv::morphologyEx(process_frame, process_frame, cv::MORPH_OPEN, kernel1);
  cv::morphologyEx(process_frame, process_frame, cv::MORPH_CLOSE, kernel2);

//  constexpr int view_count = 8;
//  cv::Mat vertical_view[view_count];
//  int view_width = m_frame_size.width / view_count;
//  for (int i = 0; i < view_count; ++i) {
//    vertical_view[i] = process_frame.colRange(i * view_width, (i + 1) * view_width - 1);
//
//  }
  std::vector<vPoints> contours;
  cv::findContours(process_frame, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
  cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 2);

  std::string str;
  if (!result.axis.empty()) {
    str = "dev: ";
    str += std::to_string(result.axis[0].x);
    str += "   deg: ";
    str += std::to_string(result.axis[0].rot);
    str += "\n";
    cv::putText(frame, str, cv::Point(0, 20), 0, 0.7f, cv::Scalar(255, 0, 0), 2);
  }
  cv::cvtColor(process_frame, process_frame, cv::COLOR_GRAY2BGR);
  cv::hconcat(frame, process_frame, result.frame);
  return result;
}
}// namespace auv::vision
