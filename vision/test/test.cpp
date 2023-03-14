//
// Created by Qff on 2023/3/5.
//

#include "../red_bar.h"
#include "../red_door.h"
#include "../utils.h"

#include <iostream>

int main() {
  //auv::vision::RedDoorBlock rdb;
  //  auv::vision::RedDoorBlock rdb;
  //  auv::vision::regulate_threshold_params("./door.mkv", rdb);
  //  cv::Mat frame = cv::imread("un.jpg");
  //  cv::imshow("un.jpg", frame);
  //  cv::waitKey(0);
  //  cv::Mat gray;
  //  cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
  //  auto frame_size = frame.size();
  //
  //  std::vector<cv::Point2f> corners;
  //  std::vector<std::vector<cv::Point2f>> corners_vec;
  //  cv::TermCriteria criteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 35, 0.001);
  //  int rt = cv::findChessboardCorners(gray, cv::Size(11, 8), corners);
  //  if (rt == 0) {
  //    std::cout << "not find corner" << std::endl;
  //    return 0;
  //  }
  //  std::cout << corners.size() << std::endl;
  //  cv::cornerSubPix(gray, corners, cv::Size(5, 5), cv::Size(-1, -1), criteria);
  //  corners_vec.push_back(corners);
  //
  //  cv::drawChessboardCorners(frame, cv::Size(11, 8), corners, true);
  //  cv::imshow("chess", frame);
  //  cv::waitKey();
  //  cv::Size square_size(30, 30);
  //  std::vector<cv::Point2f> temp_points11;
  //  for (int i = 0; i < 8; ++i) {
  //    for (int j = 0; j < 11; ++j) {
  //      cv::Point2f real_point{j * 1.0f * square_size.width, i * 1.0f * square_size.height};
  //      temp_points11.push_back(real_point);
  //    }
  //  }
  // std::vector<std::vector<cv::Point3f>> object_points(1, temp_points11);
  auto cap = cv::VideoCapture("red_bar.mkv");

  std::vector<cv::Point2f> src{
      cv::Point2f(220, 112),
      cv::Point2f(420, 112),
      cv::Point2f(0, 480),
      cv::Point2f(640, 480),
  };
  std::vector<cv::Point2f> dst{
      cv::Point2f(100, 0),
      cv::Point2f(540, 0),
      cv::Point2f(100, 480),
      cv::Point2f(540, 480),
  };

  cv::Mat trans_mat = cv::getPerspectiveTransform(src, dst);
  cv::Mat frame;
  auv::vision::RedBarBlock block;
  while (cap.read(frame)) {
    cv::Mat result;
    cv::warpPerspective(frame, result, trans_mat, frame.size()); //INTER_CUBIC
    auto re = block.process(frame);
    cv::imshow("test", re.frame);
    cv::waitKey(0);
  }

  return 0;
}
