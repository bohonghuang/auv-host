//
// Created by Qff on 2023/3/4.
//

#ifndef UTILS_H
#define UTILS_H

#include "block.h"
#include <opencv2/opencv.hpp>

namespace auv::vision {

template<typename V, typename T>
V get_point_dist(cv::Point_<T> p1, cv::Point_<T> p2) {
  auto [x1, y1] = p1;
  auto [x2, y2] = p2;
  auto x_dist_2 = cv::pow(x1 - x2, 2);
  auto y_dist_2 = cv::pow(y1 - y2, 2);
  return cv::pow(x_dist_2 + y_dist_2, 0.5);
}

template<typename T, size_t N>
cv::Point_<T> get_point_center(std::array<cv::Point_<T>, N> points) {
  double count_x = 0.0;
  double count_y = 0.0;
  for (const auto &i: points) {
    count_x += i.x;
    count_y += i.y;
  }
  return {static_cast<T>(count_x / N),
          static_cast<T>(count_y / N)};
}

template<typename T>
cv::Point_<T> get_point_center(std::vector<cv::Point_<T>> points) {
  double count_x = 0.0;
  double count_y = 0.0;
  for (const auto &i: points) {
    count_x += i.x;
    count_y += i.y;
  }
  return {static_cast<T>(count_x / points.size()),
          static_cast<T>(count_y / points.size())};
}

template<typename T>
float get_point_deg_in_top_axis(cv::Point_<T> p1, cv::Point_<T> p2) {
  auto [x1, y1] = p1;          //    ·---------------> x
  auto [x2, y2] = p2;          //    |
  auto dx = x2 - x1;           //    |
  auto dy = y2 - y1;           //    |
  return cv::fastAtan2(dx, dy);//     v y
};

template<typename T>
float get_point_deg_in_bottom_axis(cv::Point_<T> p1, cv::Point_<T> p2) {
  return 180.0f - get_point_deg_in_top_axis(p1, p2);
};

void regulate_threshold_params(std::string_view file_name, ThresholdBlock<int> &block);

}// namespace auv::vision

#endif//UTILS_H
