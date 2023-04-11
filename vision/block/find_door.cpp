#include "find_door.h"
#include "utils.h"

namespace auv::vision {

FindLineBlock::FindLineBlock(double rho, double theta, int threshold)
    : m_rho(rho), m_theta(theta), m_threshold(threshold) {
}

struct HoughResult {
  double distance = 0.0;
  double theta = 0.0;
  int count = 0;
};

static void lines_update(HoughResult &result, double distance, double deg) {
  result.distance += distance;
  result.theta += deg;
  result.count += 1;
}

static void lines_process(HoughResult &result) {
  if (result.count == 0)
    return;

  result.distance /= result.count;
  result.theta /= result.count;
}

static std::vector<std::array<cv::Point2f, 4>> lines_get_bar(const cv::Mat &frame, cv::Mat &preview_frame, const HoughResult &line_params) {
  if(line_params.count == 0)
    return {};
  cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
  double cos_theta = cos(line_params.theta), sin_theta = sin(line_params.theta);
  double x0 = cos_theta * line_params.distance, y0 = sin_theta * line_params.distance;
  cv::Point pt1, pt2;
  pt1.x = cvRound(x0 - 1000 * sin_theta);
  pt1.y = cvRound(y0 + 1000 * cos_theta);
  pt2.x = cvRound(x0 + 1000 * sin_theta);
  pt2.y = cvRound(y0 - 1000 * cos_theta);
  cv::line(preview_frame, pt1, pt2, cv::Scalar{255, 255, 0}, 30, cv::LINE_AA);
  cv::line(mask, pt1, pt2, cv::Scalar{255, 255, 255}, 30, cv::LINE_AA);
  cv::bitwise_and(frame, mask, mask);
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask, contours, cv::noArray(),
                   cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  std::vector<std::array<cv::Point2f, 4>> results;
  for (const auto &contour: contours) {
    cv::Point2f rect_points[4];
    auto rect = cv::minAreaRect(contour);
    rect.points(rect_points);
    for (size_t i = 0; i < 4; i++)
      cv::line(preview_frame, rect_points[i], rect_points[(i + 1) % 4],
               cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
    results.push_back({rect_points[0], rect_points[1], rect_points[2], rect_points[3]});
  }
  return results;
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

  std::vector<cv::Vec2f> lines;
  cv::HoughLines(frame, lines, m_rho, m_theta, m_threshold);

  HoughResult bottom, left, right;
  for (const auto &i: lines) {
    float distance = i[0], deg = i[1];
    if (std::fabs(deg) > utils::deg2rad(50.0)) {// 门的两侧直线
      if (std::abs(distance) < (float) width / 2) {
        lines_update(left, distance, deg);
      } else {
        lines_update(right, distance, deg);
      }
    } else {// 门的底部直线
      lines_update(bottom, distance, deg);
    }
  }

  lines_process(left);
  lines_process(right);
  lines_process(bottom);

  auto left_result = lines_get_bar(frame, preview_frame, left);
  auto right_result = lines_get_bar(frame, preview_frame, right);
  auto bottom_result = lines_get_bar(frame, preview_frame, bottom);

  std::vector<std::array<cv::Point2f, 4>> results;
  results.insert(results.end(), left_result.begin(), left_result.end());
  results.insert(results.end(), right_result.begin(), right_result.end());
  results.insert(results.end(), bottom_result.begin(), bottom_result.end());

  for (auto &it: results) {
    for(auto &i : it) {
      static const int half_width = width / 2;
      static const int half_height = height / 2;
      i.x = (i.x - (float) half_width) / (float) half_width;
      i.y = -(i.y - (float) half_height) / (float) half_height;
    }
  }
  return {preview_frame, std::move(results)};
}

}// namespace auv::vision
