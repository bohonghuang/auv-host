//
// Created by Qff on 2023/3/5.
//

#include "red_door.h"
#include "utils.h"

namespace auv::vision {

RedDoorBlock::RedDoorBlock(Camera &camera) {
  m_threshold_params = {
      {33, 177},
      {146, 255},
      {65, 130}};
}

struct ORBParam {
  int nfeatures = 500;
  float scaleFactor = 1.2f;
  int nlevels = 8;
  int edgeThreshold = 31;
  int firstLevel = 0;
  int WTA_K = 2;
  cv::ORB::ScoreType scoreType = cv::ORB::HARRIS_SCORE;
  int patchSize = 31;
  int fastThreshold = 20;

  cv::DescriptorMatcher::MatcherType descriptor_matcher = cv::DescriptorMatcher::MatcherType::BRUTEFORCE_HAMMING;
};

AlgorithmResult RedDoorBlock::process_imp(const cv::Mat &frame, auv::vision::TimeStep ts) noexcept {
  AlgorithmResult result;

  cv::Mat process_frame = frame.clone();
  cv::cvtColor(process_frame, process_frame, cv::COLOR_BGR2YCrCb);
  cv::inRange(process_frame, get_low_param(), get_high_param(), process_frame);

  int high = m_frame_size.height;
  static cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                     cv::Size(high / 52, high / 52));
  static cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                     cv::Size(high / 24, high / 24));
  cv::morphologyEx(process_frame, process_frame, cv::MORPH_OPEN, kernel1);
  cv::morphologyEx(process_frame, process_frame, cv::MORPH_CLOSE, kernel2);

  //  static ORBParam orb_param;
  //  static cv::Ptr<cv::ORB> orb_detector = cv::ORB::create(orb_param.nfeatures, orb_param.scaleFactor, orb_param.nlevels,
  //                                                        orb_param.edgeThreshold, orb_param.firstLevel, orb_param.WTA_K,
  //                                                        orb_param.scoreType, orb_param.patchSize, orb_param.fastThreshold);
  //  static cv::Ptr<cv::DescriptorMatcher> s_orb_descriptor_matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::MatcherType::BRUTEFORCE_HAMMING);
  //  static cv::Mat door = cv::imread("door.png");
  //  static std::tuple<std::vector<cv::KeyPoint>, cv::Mat> door_keypoint_and_descriptor = []() {
  //    std::vector<cv::KeyPoint> key_points;
  //    cv::Mat descriptor;
  //    orb_detector->detectAndCompute(door, cv::Mat(), key_points, descriptor);
  //    return std::tuple{key_points, descriptor};
  //  }();
  //
  //  std::vector<cv::KeyPoint> frame_keypoint;
  //  cv::Mat frame_descriptors;
  //  orb_detector->detectAndCompute(frame, cv::Mat(), frame_keypoint, frame_descriptors);
  //
  //  std::vector<cv::DMatch> matches;
  //  cv::Mat output_frame;
  //  s_orb_descriptor_matcher->match(frame_descriptors, std::get<1>(door_keypoint_and_descriptor), matches, cv::Mat());
  //  cv::drawMatches(frame, frame_keypoint, door, std::get<0>(door_keypoint_and_descriptor),
  //                  matches, output_frame);
  //  cv::imshow("testtss", output_frame);


  //std::vector<cv::Vec2f> lines;
  //  std::vector<cv::Vec4i> lines;
  //  cv::HoughLinesP(process_frame, lines, 1, CV_PI / 180, 80, 30, 10);
  //  for(auto & line : lines)
  //  {
  //    // std::cout << line[0] << "  " << line[1] << "   " << line[2] << "  " << line[3] << std::endl;
  //    cv::line( frame, cv::Point(line[0], line[1]),
  //             cv::Point( line[2], line[3]), cv::Scalar(0,0,255), 3, 8 );
  //  }
  //  for (auto &line: lines) {
  //    float rho = line[0], theta = line[1];
  //    cv::Point pt1, pt2;
  //    double a = cos(theta), b = sin(theta);
  //    double x0 = a * rho, y0 = b * rho;
  //    pt1.x = cvRound(x0 + 1000 * (-b));
  //    pt1.y = cvRound(y0 + 1000 * (a));
  //    pt2.x = cvRound(x0 - 1000 * (-b));
  //    pt2.y = cvRound(y0 - 1000 * (a));
  //    cv::line(frame, pt1, pt2, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
  //  }
  //  constexpr int view_count = 8;
  //  cv::Mat vertical_view[view_count];
  //  int view_width = m_frame_size.width / view_count;
  //  for (int i = 0; i < view_count; ++i) {
  //    vertical_view[i] = process_frame.colRange(i * view_width, (i + 1) * view_width - 1);
  //
  //  }
  //  std::vector<vPoints> contours;
  //  cv::findContours(process_frame, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
  //  cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 2);

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
