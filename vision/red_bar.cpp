//
// Created by Qff on 2023/3/4.
//

#include "red_bar.h"
#include "utils.h"

namespace auv::vision {

RedBarBlock::RedBarBlock() {
  m_threshold_params = {
      {33, 177},  // Y
      {146, 255}, // Cr
      {65, 130}}; // Cb
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

  for (const auto &contour: contours) {
    cv::Point2f rect_points[4];
    auto rect = cv::minAreaRect(contour);
    rect.points(rect_points);
    auto dist01 = get_point_dist<double>(rect_points[0], rect_points[1]);
    auto dist12 = get_point_dist<double>(rect_points[1], rect_points[2]);
    auto [width, length] = std::minmax({dist01, dist12});

    // std::cout << "length:" << length << "   width:" << width << std::endl;
    if (width < (double) high / 12) {
      // std::cout << "width" << std::endl;
      continue;
    }
    if (length < (float) high / 8) {
      // std::cout << "length < (float) high" << std::endl;
      continue;
    }
    if (length / width > 6) {
      // std::cout << "length / width > 6" << std::endl;
      continue;
    }

    cv::Mat mask = cv::Mat::zeros(m_frame_size, CV_8UC1);
    cv::fillPoly(mask, transform_points(rect_points), cv::Scalar(255, 255, 255));
    cv::Mat mask_result;
    cv::bitwise_and(process_frame, mask, mask_result);
    auto fill_percent = (double) cv::countNonZero(mask_result) / cv::countNonZero(mask);
    if (fill_percent < 0.6f)
      continue;

    float deg;
    if (dist01 > dist12)
      deg = get_point_deg_in_bottom_axis(rect_points[0], rect_points[1]);
    else
      deg = get_point_deg_in_bottom_axis(rect_points[1], rect_points[2]);
    deg = 90.0f - deg;
    if (std::abs(deg) > 60)
      continue;

    for (size_t i = 0; i < 4; i++)
      line(frame, rect_points[i], rect_points[(i + 1) % 4],
           cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
    cv::Point cent_point = get_point_center(contour);
    auto dev = static_cast<float>((double) cent_point.x / m_frame_size.width - 0.5);

    result.axis.emplace_back(
        dev,
        0.0f,
        0.0f,
        deg / 90.0f);
  }

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