#include "find_bar.h"

#include "utils.h"

namespace auv::vision {

FindBarBlock::FindBarBlock(bool draw_contours)
    : m_draw_contours(draw_contours) {}

FindBarBlock::Out FindBarBlock::process(cv::Mat frame) {
  if (frame.channels() != 1) {
    std::cout << "The frame need gray color type! But it is " << frame.channels() << std::endl;
    return {};
  }

  cv::Mat preview;
  if (m_draw_contours) {
    preview = frame.clone();
    cv::cvtColor(preview, preview, cv::COLOR_GRAY2BGR);
  }
  cv::Mat process_frame = frame.clone();
  int height = frame.size().height;
  static cv::Mat kernel1 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(height / 96, height / 96));
  static cv::Mat kernel2 =
      cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                cv::Size(height / 48, height / 48));
  cv::morphologyEx(process_frame, process_frame, cv::MORPH_OPEN, kernel1);
  cv::morphologyEx(process_frame, process_frame, cv::MORPH_CLOSE, kernel2);

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(process_frame, contours, cv::noArray(),
                   cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
  std::sort(contours.begin(), contours.end(),
            [](const std::vector<cv::Point> &points1, const std::vector<cv::Point> &points2) {
              cv::Point p1 = utils::get_point_center(points1);
              cv::Point p2 = utils::get_point_center(points2);
              return p1.y < p2.y;
            });

  std::vector<FindBarResult> results;
  results.resize(contours.size());
  for (const auto &contour: contours) {
    cv::Point2f rect_points[4];
    auto rect = cv::minAreaRect(contour);
    rect.points(rect_points);
    auto dist01 = utils::get_point_dist<double>(rect_points[0], rect_points[1]);
    auto dist12 = utils::get_point_dist<double>(rect_points[1], rect_points[2]);
    auto [width, length] = std::minmax({dist01, dist12});

    // std::cout << "length:" << length << "   width:" << width << std::endl;
    if (width < (double) height / 12) {
      // std::cout << "width" << std::endl;
      continue;
    }
    if (length < (float) height / 8) {
      // std::cout << "length < (float) high" << std::endl;
      continue;
    }
    if (length / width > 6) {
      // std::cout << "length / width > 6" << std::endl;
      continue;
    }

    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillPoly(mask, utils::transform_points(rect_points), cv::Scalar(255, 255, 255));
    cv::Mat mask_result;
    cv::bitwise_and(process_frame, mask, mask_result);
    auto fill_percent = (double) cv::countNonZero(mask_result) / cv::countNonZero(mask);
    if (fill_percent < 0.6f)
      continue;

    float deg;
    if (dist01 > dist12)
      deg = utils::get_point_deg_in_bottom_axis(rect_points[0], rect_points[1]);
    else
      deg = utils::get_point_deg_in_bottom_axis(rect_points[1], rect_points[2]);
    deg = 90.0f - deg;
    if (std::abs(deg) > 60)
      continue;

    if (m_draw_contours) {
      for (size_t i = 0; i < 4; i++)
        line(preview, rect_points[i], rect_points[(i + 1) % 4],
             cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
    }
    cv::Point cent_point = utils::get_point_center(contour);
    //    auto dev = static_cast<float>((double) cent_point.x / frame.size().width - 0.5);
    results.push_back({cent_point, deg});
  }

  return {preview,
          results};
}

}// namespace auv::vision
