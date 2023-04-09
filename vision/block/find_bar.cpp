#include "find_bar.h"

#include "utils.h"

namespace auv::vision {

FindBarBlock::FindBarBlock(bool debug)
    : m_debug(debug) {}

FindBarBlock::Out
FindBarBlock::process(cv::Mat frame) {
  cv::Mat preview;
  if (m_debug) {
    preview = frame.clone();
    cv::cvtColor(preview, preview, cv::COLOR_GRAY2BGR);
  }

  static const int height = frame.size().height;
  static const cv::Mat kernel1 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(height / 96, height / 96));
  static const cv::Mat kernel2 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(height / 48, height / 48));
  cv::morphologyEx(frame, frame, cv::MORPH_OPEN, kernel1);
  cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, kernel2);

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(frame, contours, cv::noArray(),
                   cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  FindBarResults output;
  auto &point_results = output.result;
  point_results.reserve(contours.size());
  for (const auto &contour: contours) {
    cv::Point2f rect_points[4];
    auto rect = cv::minAreaRect(contour);
    rect.points(rect_points);

    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillPoly(mask, std::vector<cv::Point> { rect_points, rect_points + 4 }, cv::Scalar(255, 255, 255));
    cv::Mat mask_result;
    cv::bitwise_and(frame, mask, mask_result);
    double non_zero_count = cv::countNonZero(mask_result);

    if (m_debug) {
      for (size_t i = 0; i < 4; i++)
        cv::line(preview, rect_points[i], rect_points[(i + 1) % 4],
                 cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
    }

    static const int frame_width = frame.size().width;
    static const int frame_height = frame.size().height;
    static const int half_width = frame_width / 2;
    static const int half_height = frame_height / 2;

    FindBarResult result;
    static const auto all_img_area = frame_width * frame_height;
    result.area = non_zero_count / all_img_area;
    for (size_t i = 0; i < 4; ++i) {
      result.points[i] = {(rect_points[i].x - (float) half_width) / (float) half_width,
                          -(rect_points[i].y - (float) half_height) / (float) half_height};
    }
    point_results.push_back(result);
  }

  if (m_debug) {
    output.frame = preview;
  }

  return output;
}

}// namespace auv::vision
