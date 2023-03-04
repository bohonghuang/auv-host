//
// Created by Qff on 2023/3/4.
//

#include "utils.h"
#include "block.h"

namespace auv::vision {

static cv::Mat s_frame;
static auto s_win_name = "RegulateThresholdParams";
static ThresholdBlock<int>* s_threshold_block;

static void on_regulate_threshold_params_change(int value, void *userdata) {
  *(int *) userdata = value;
  auto result = s_threshold_block->process(s_frame.clone());
  cv::imshow(s_win_name, result.frame);
}

void regulate_threshold_params(std::string_view file_name, ThresholdBlock<int> &block) {
  cv::namedWindow(s_win_name);
  s_threshold_block = &block;
  auto& threshold = block.test_get_threshold_params();
  cv::createTrackbar("1 Low", s_win_name, &threshold.param1.low,
                     255, on_regulate_threshold_params_change, &threshold.param1.low);
  cv::createTrackbar("2 Low", s_win_name, &threshold.param2.low,
                     255, on_regulate_threshold_params_change, &threshold.param2.low);
  cv::createTrackbar("3 Low", s_win_name, &threshold.param3.low,
                     255, on_regulate_threshold_params_change, &threshold.param3.low);
  cv::createTrackbar("1 Up", s_win_name, &threshold.param1.high,
                     255, on_regulate_threshold_params_change, &threshold.param1.high);
  cv::createTrackbar("2 Up", s_win_name, &threshold.param2.high,
                     255, on_regulate_threshold_params_change, &threshold.param2.high);
  cv::createTrackbar("3 Up", s_win_name, &threshold.param3.high,
                     255, on_regulate_threshold_params_change, &threshold.param3.high);

  cv::VideoCapture cap(file_name.data());
  while (cap.read(s_frame)) {
    auto result = block.process(s_frame.clone());
    cv::imshow(s_win_name, result.frame);
    auto key = cv::waitKey();
    switch (key) {
      case 'q':
        break;
      case 's':
        for (size_t i = 0; i < 10; ++i)
          cap.read(s_frame);
      default:
        continue;
    }
  }
}

}// namespace auv::vision