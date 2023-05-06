#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <cmath>

namespace auv::vision::utils {

template<typename V, typename T>
V get_point_dist(const cv::Point_<T> &p1, const cv::Point_<T> &p2) {
  auto [x1, y1] = p1;
  auto [x2, y2] = p2;
  auto x_dist_2 = cv::pow(x1 - x2, 2);
  auto y_dist_2 = cv::pow(y1 - y2, 2);
  return cv::pow(x_dist_2 + y_dist_2, 0.5);
}


template<typename R = float, typename T>//vector array
cv::Point_<R> get_point_center(const T &array) {
  R count_x = 0.0;
  R count_y = 0.0;
  size_t size = 0;
  for (const auto &i: array) {
    count_x += i.x;
    count_y += i.y;
    size++;
  }
  return {count_x / size, count_y / size};
}


template<typename T>
float get_point_deg(const cv::Point_<T> &p1, const cv::Point_<T> &p2, bool from_bottom = false) {
  auto [x1, y1] = p1;
  auto [x2, y2] = p2;
  auto dx = x2 - x1;
  auto dy = y2 - y1;
  auto result = cv::fastAtan2(dy, dx);
  result = result > 180.0f ? result - 180.0f : result;
  if (from_bottom) result = 180.0f - result;
  return result;
}

template<typename T>
std::array<cv::Point_<T>, 4>
get_top_left_right_bottom_left_right_point(const cv::Point_<T> (&four_point)[4]) {
  std::array<cv::Point_<T>, 4> result;
  std::copy(std::begin(four_point), std::end(four_point), result.begin());
  std::sort(result.begin(), result.end(), [](const cv::Point_<T> &p1, const cv::Point_<T> &p2) {
    return p1.y < p2.y;
  });
  if (result[0].x > result[1].x)
    std::swap(result[0], result[1]);
  if (result[2].x > result[3].x)
    std::swap(result[2], result[3]);
  return result;
}


template<typename T>
std::vector<cv::Point2d>
normalize_points(const T &points, cv::Size frame_size) {
  std::vector<cv::Point2d> results;
  results.reserve(points.size());
  double half_x = (double) frame_size.width / 2;
  double half_y = (double) frame_size.height / 2;
  for (const auto &point: points) {
    results.emplace_back((double) (point.x - half_x) / half_x,
                         -(double) (point.y - half_y) / half_y);
  }
  return results;
}

inline double rad2deg(double rad) {
    return 180.0 * rad / M_PI;
}

inline double deg2rad(double deg) {
    return 2 * M_PI * deg / 360.0;
}

//void regulate_threshold_params(std::string_view file_name, ThresholdBlock<int> &block);

std::vector<std::string>
split(std::string_view str, std::string_view delim = " ");



}// namespace auv::vision::utils

#endif//UTILS_H
