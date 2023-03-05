//
// Created by Qff on 2023/3/4.
//

#ifndef UTILS_H
#define UTILS_H

#include "block.h"
#include <opencv2/opencv.hpp>

namespace auv::vision {

template<typename V, typename T>
V get_point_dist(const cv::Point_<T> &p1, const cv::Point_<T> &p2) {
  auto [x1, y1] = p1;
  auto [x2, y2] = p2;
  auto x_dist_2 = cv::pow(x1 - x2, 2);
  auto y_dist_2 = cv::pow(y1 - y2, 2);
  return cv::pow(x_dist_2 + y_dist_2, 0.5);
}


template<typename T, size_t N>
auto get_point_center(const std::array<cv::Point_<T>, N> &points)
    -> cv::Point_<T> {
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
auto get_point_center(const std::vector<cv::Point_<T>> &points)
    -> cv::Point_<T> {
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
float get_point_deg_in_top_axis(const cv::Point_<T> &p1, const cv::Point_<T> &p2) {
  auto [x1, y1] = p1;
  auto [x2, y2] = p2;
  auto dx = x2 - x1;
  auto dy = y2 - y1;
  auto result = cv::fastAtan2(dy, dx);
  return result > 180.0f ? result - 180.0f : result;
};


template<typename T>
float get_point_deg_in_bottom_axis(const cv::Point_<T> &p1, const cv::Point_<T> &p2) {
  return 180.0f - get_point_deg_in_top_axis(p1, p2);
};


template<typename T, size_t N>
auto transform_points(cv::Point_<T> (&src)[N])
    -> std::vector<cv::Point> {
  std::vector<cv::Point> results;
  results.resize(4);
  for (size_t i = 0; i < N; ++i)
    results[i] = src[i];
  return results;
}


template<typename T>
auto get_top_left_right_bottom_left_right_point(const cv::Point_<T> (&four_point)[4])
    -> std::array<cv::Point_<T>, 4> {
  std::array<cv::Point_<T>, 4> result;
  std::copy(std::begin(four_point), std::end(four_point), result.begin());
  std::sort(result.begin(), result.end(), [](const cv::Point_<T> &p1, const cv::Point_<T> &p2) {
    return p1.y < p2.y;
  });
  if(result[0].x > result[1].x)
    std::swap(result[0], result[1]);
  if(result[2].x > result[3].x)
    std::swap(result[2], result[3]);
  return result;
}


void regulate_threshold_params(std::string_view file_name, ThresholdBlock<int> &block);

}// namespace auv::vision

#endif//UTILS_H
