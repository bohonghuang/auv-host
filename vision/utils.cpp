//
// Created by Qff on 2023/3/4.
//

#include "utils.h"
#include "block.h"

namespace auv::vision::utils {

static cv::Mat s_frame;
static auto s_win_name = "RegulateThresholdParams";
static ThresholdBlock<int> *s_threshold_block;

static void on_regulate_threshold_params_change(int value, void *userdata) {
  const char *trackbar_name = (char *) userdata;
  if (!strcmp(trackbar_name, "1 Low"))
    s_threshold_block->test_get_threshold_params().param1.low = value;
  else if (!strcmp(trackbar_name, "2 Low"))
    s_threshold_block->test_get_threshold_params().param2.low = value;
  else if (!strcmp(trackbar_name, "3 Low"))
    s_threshold_block->test_get_threshold_params().param3.low = value;
  else if (!strcmp(trackbar_name, "1 Up"))
    s_threshold_block->test_get_threshold_params().param1.high = value;
  else if (!strcmp(trackbar_name, "2 Up"))
    s_threshold_block->test_get_threshold_params().param2.high = value;
  else if (!strcmp(trackbar_name, "3 Up"))
    s_threshold_block->test_get_threshold_params().param3.high = value;
  auto result = s_threshold_block->process(s_frame.clone());
  cv::imshow(s_win_name, result.frame);
}

void regulate_threshold_params(std::string_view file_name, ThresholdBlock<int> &block) {
  cv::namedWindow(s_win_name);
  s_threshold_block = &block;
  auto &threshold = block.test_get_threshold_params();
#define XX(name)                                \
  cv::createTrackbar(name, s_win_name, nullptr, \
                     255, on_regulate_threshold_params_change, (void *) name);
  XX("1 Low");
  XX("2 Low");
  XX("3 Low");
  XX("1 Up");
  XX("2 Up");
  XX("3 Up");

#undef XX

  cv::VideoCapture cap(file_name.data());
  while (cap.read(s_frame)) {
    auto &params = s_threshold_block->test_get_threshold_params();
    cv::setTrackbarPos("1 Low", s_win_name, params.param1.low);
    cv::setTrackbarPos("2 Low", s_win_name, params.param2.low);
    cv::setTrackbarPos("3 Low", s_win_name, params.param3.low);
    cv::setTrackbarPos("1 Up", s_win_name, params.param1.high);
    cv::setTrackbarPos("2 Up", s_win_name, params.param2.high);
    cv::setTrackbarPos("3 Up", s_win_name, params.param3.high);
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

std::vector<std::string> split(std::string_view str, std::string_view delim) {
  std::vector<std::string> results;
  size_t begin = 0;
  size_t pos = str.find(delim);
  while (pos < str.length()) {
    size_t length = pos - begin;
    results.emplace_back(str.substr(begin, length));
    begin = pos + delim.length();
    pos = str.find(delim, begin);
  }
  if (str.length() - begin)
    results.emplace_back(str.substr(begin, str.length() - begin));
  return results;
}

}// namespace auv::vision