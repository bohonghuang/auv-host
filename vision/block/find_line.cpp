#include "find_line.h"

namespace auv::vision {

FindLineBlock::FindLineBlock(double rho, double theta, int threshold)
    : m_rho(rho), m_theta(theta), m_threshold(threshold) {
}

FindLineResults FindLineBlock::process(cv::Mat frame) {
  std::vector<cv::Vec2f> lines;
  cv::HoughLines(frame, lines, m_rho, m_theta, m_threshold);

  std::vector<FindLineResult> result;
  result.reserve(lines.size());

  auto preview_frame = frame.clone();
  cv::cvtColor(preview_frame, preview_frame, cv::COLOR_GRAY2BGR);
  for (const auto &i: lines) {
    float rho = i[0], theta = i[1];
    double a = cos(theta), b = sin(theta);
    double x0 = a * rho, y0 = b * rho;
    // result.push_back({{x0 - 1000 * b, y0 + 1000 * a}, {x0 + 1000 * b, y0 - 1000 * a}});
    result.push_back({M_PI/2 - theta, {x0, y0}});

    // std::cout << x0/640.0 << "  " << y0/480.0 << std::endl;

    cv::Point pt1, pt2;
    pt1.x = cvRound(x0 - 1000*b);
    pt1.y = cvRound(y0 + 1000*a);
    pt2.x = cvRound(x0 + 1000*b);
    pt2.y = cvRound(y0 - 1000*a);
    cv::line(preview_frame, pt1, pt2, cv::Scalar(255,0,255), 3, cv::LINE_AA);
  }

  return {preview_frame, result};
}

}// namespace auv::vision
