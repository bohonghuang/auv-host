//
// Created by qff233 on 23-3-22.
//
#include "find_ball.h"

auv::vision::FindBallBlock::FindBallBlock(bool debug)
    : m_debug(debug) {
}


auv::vision::FindBallResults
auv::vision::FindBallBlock::process(cv::Mat frame) {
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

  FindBallResults output;
  auto &point_results = output.result;
  point_results.reserve(contours.size());
  for (const auto &contour: contours) {
    cv::Point2f rect_points[4];
    auto rect = cv::minAreaRect(contour);
    rect.points(rect_points);

    if (m_debug) {
      for (size_t i = 0; i < 4; ++i) {
        cv::line(preview, rect_points[i], rect_points[(i + 1) % 4],
                 cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
      }
      output.frame = preview;
    }

    static const int frame_width = frame.size().width;
    static const int frame_height = frame.size().height;
    static const int half_width = frame_width / 2;
    static const int half_height = frame_height / 2;

    FindBallResult result;
    for (size_t i = 0; i < 4; ++i) {
      result.points[i] = {
          (rect_points[i].x - (float) half_width) / (float) half_width,
          -(rect_points[i].y - (float) half_height) / (float) half_height};
    }
    point_results.push_back(result);
  }
  return output;
}
