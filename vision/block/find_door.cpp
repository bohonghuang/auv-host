#include "find_door.h"
#include <algorithm>
#include <opencv2/core/matx.hpp>
#include <opencv2/imgproc.hpp>

namespace auv::vision {

FindLineBlock::FindLineBlock(double rho, double theta, int threshold)
    : m_rho(rho), m_theta(theta), m_threshold(threshold) {
}

FindDoorResults FindLineBlock::process(cv::Mat frame) {
  static const int height = frame.size().height;
  static const int width = frame.size().width;
  static const cv::Mat kernel1 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(height / 96, height / 96));
  static const cv::Mat kernel2 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(height / 48, height / 48));
  cv::morphologyEx(frame, frame, cv::MORPH_OPEN, kernel1);
  cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, kernel2);

  cv::Mat preview_frame = frame.clone();
  cv::cvtColor(preview_frame, preview_frame, cv::COLOR_GRAY2BGR);

  std::vector<cv::Vec4i> lines;
  cv::HoughLinesP(frame, lines, m_rho, m_theta, m_threshold, 75, 6);
  // for (const auto &i: lines) {
  //   cv::line(preview_frame, cv::Point(i[0], i[1]),
  //            cv::Point(i[2], i[3]), cv::Scalar(255, 0, 0), 1, 8);
  // }

  std::array<float, 4> left = {0, 0, 0, 0};
  size_t left_count = 0;
  std::array<float, 4> right = {0, 0, 0, 0};
  size_t right_count = 0;
  std::array<float, 4> bottom = {0, 0, 0, 0};
  size_t bottom_count = 0;

  for (auto &i: lines) {
    auto [tmp] = i;
    auto [p1_x, p1_y, p2_x, p2_y] = tmp;

    auto update = [](std::array<float, 4> &pos, size_t &count, const cv::Vec4i &val) {
      pos[0] += val[0];
      pos[1] += val[1];
      pos[2] += val[2];
      pos[3] += val[3];
      count++;
    };

    if (p1_x == p2_x) {
      auto x_center = (p1_x + p2_x) / 2;
      if (p1_y > p2_y) {
        std::swap(i[1], i[3]);
        std::swap(i[0], i[2]);
      }
      if (x_center > width / 2) {
        update(right, right_count, i);
      } else {
        update(left, left_count, i);
      }
    } else {
      auto k = (float) (p1_y - p2_y) / (p1_x - p2_x);
      if (std::fabs(k) < 1) {
        if (p1_x > p2_x) {
          std::swap(i[1], i[3]);
          std::swap(i[0], i[2]);
        }
        update(bottom, bottom_count, i);
      } else {
        auto x_center = (p1_x + p2_x) / 2;
        if (p1_y > p2_y) {
          std::swap(i[1], i[3]);
          std::swap(i[0], i[2]);
        }
        if (x_center > width / 2) {
          update(right, right_count, i);
        } else {
          update(left, left_count, i);
        }
      }
    }
  }

  if (left_count + right_count + bottom_count != lines.size())
    std::cout << "asadasd1!!! "
              << left_count << "  "
              << bottom_count << "  "
              << right_count << "  "
              << lines.size() << "  "
              << std::endl;

  if (left_count != 0)
    std::for_each(left.begin(), left.end(), [left_count](float &val) { val /= left_count; });
  if (right_count != 0)
    std::for_each(right.begin(), right.end(), [right_count](float &val) { val /= right_count; });
  if (bottom_count != 0)
    std::for_each(bottom.begin(), bottom.end(), [bottom_count](float &val) { val /= bottom_count; });

  cv::line(preview_frame, cv::Point(left[0], left[1]),
           cv::Point(left[2], left[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
  cv::line(preview_frame, cv::Point(right[0], right[1]),
           cv::Point(right[2], right[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
  cv::line(preview_frame, cv::Point(bottom[0], bottom[1]),
           cv::Point(bottom[2], bottom[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);

  auto normalize = [](std::array<float, 4> &val) {
    auto half_x = (float) width / 2;
    auto half_y = (float) height / 2;
    val[0] = (val[0] - half_x) / half_x;
    val[2] = (val[2] - half_x) / half_x;
    val[1] = -(val[1] - half_y) / half_y;
    val[3] = -(val[3] - half_y) / half_y;
  };

  if (left_count != 0)
    normalize(left);
  if (right_count != 0)
    normalize(right);
  if (bottom_count != 0)
    normalize(bottom);

  return {preview_frame, {left, right, bottom}};
}

}// namespace auv::vision
