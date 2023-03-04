//
// Created by Qff on 2023/3/4.
//

#include "red_bar.h"
#include "utils.h"

namespace auv::vision {

RedBarBlock::RedBarBlock() {
  m_threshold_params = {
      {33, 177},
      {146, 255},
      {65, 130}};
}

AlgorithmResult RedBarBlock::process_imp(const cv::Mat &frame, auv::vision::TimeStep ts) noexcept {
  AlgorithmResult result;
  auto process_frame = frame.clone();
  cv::cvtColor(process_frame, process_frame, cv::COLOR_BGR2YCrCb);
  cv::inRange(process_frame, get_low_param(), get_high_param(), process_frame);

  int high = m_frame_size.height;
  static cv::Mat kernel1 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(high / 96, high / 96));
  static cv::Mat kernel2 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(high / 48, high / 48));
  cv::morphologyEx(process_frame, process_frame, cv::MORPH_OPEN, kernel1);
  cv::morphologyEx(process_frame, process_frame, cv::MORPH_CLOSE, kernel2);

  std::vector<vPoints> contours;
  cv::findContours(process_frame, contours, cv::noArray(),
                   cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
  std::sort(contours.begin(), contours.end(),
            [](const vPoints &points1, const vPoints &points2) {
              cv::Point p1 = get_point_center(points1);
              cv::Point p2 = get_point_center(points2);
              return p1.y < p2.y;
            });

  for (const auto &i: contours) {
    aPointfs<4> rect_4point;
    auto rect = cv::minAreaRect(i);
    rect.points(rect_4point.data());
    auto dist01 = get_point_dist<float>(rect_4point[0], rect_4point[1]);
    auto dist12 = get_point_dist<float>(rect_4point[1], rect_4point[2]);
    auto [length, width] = std::minmax({dist01, dist12});
    if (width < (float) high / 12 || length < (float) high / 8 || length / width > 6)
      continue;

    std::vector<cv::Point_<int>> rect_point;
    rect_point.resize(4);
    for (size_t j = 0; j < 4; ++j) {
      rect_point[j] = rect_4point[j];
    }
    cv::Mat mask = cv::Mat::zeros(m_frame_size, CV_8UC1);
    cv::fillPoly(mask, rect_point, cv::Scalar(255, 255, 255));
    cv::Mat mask_result;
    cv::bitwise_and(process_frame, mask, mask_result);
    float fill_percent = (float) cv::countNonZero(mask_result) / (float) cv::countNonZero(mask);
    if (fill_percent < 0.6f)
      continue;

    float deg;
    if (dist01 > dist12)
      deg = get_point_deg_in_bottom_axis(rect_4point[0], rect_4point[1]);
    else
      deg = get_point_deg_in_bottom_axis(rect_4point[1], rect_4point[2]);
    deg = 90.0f - deg;
    if (std::abs(deg) > 50)
      continue;

    std::vector<vPoints> points;
    points.push_back(rect_point);
    cv::drawContours(frame, points, 0,
                     cv::Scalar(0, 255, 255), 2);
    cv::Point cent_point = get_point_center(rect_4point);
    auto dev = static_cast<float>((double) cent_point.x / m_frame_size.width - 0.5);

    result.axis.emplace_back(
        dev,
        0.0f,
        0.0f,
        deg);
  }
  cv::cvtColor(process_frame, process_frame, cv::COLOR_GRAY2BGR);
  cv::hconcat(frame, process_frame, result.frame);
  return result;
}


}// namespace auv::vision