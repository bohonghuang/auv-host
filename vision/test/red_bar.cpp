//
// Created by Qff on 2023/3/4.
//

#include "../red_bar.h"
#include "../utils.h"

int main() {
  //  cv::VideoCapture cap("./line_1.mkv");
  //  cv::Mat frame;
  //  std::cout << "start..." << std::endl;
  auv::vision::RedBarBlock rdb;
  //  while (cap.read(frame)) {
  //    auto result = rdb.process(frame);
  //    cv::imshow("pre", result.frame);
  //    cv::waitKey(1);
  //  }
  auv::vision::regulate_threshold_params("./line_1.mkv", rdb);
  return 0;
}
