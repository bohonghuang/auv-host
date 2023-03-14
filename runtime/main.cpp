//
// Created by Qff on 2023/2/27.
//

#include <fstream>

#include "../vision/red_bar.h"
#include "application.h"

int main() {
  struct Vars {
    int boop = 0;
  };

  Vars vars;

  auv::Application app([](sol::state &state) {
    state.new_usertype<auv::vision::Block>(
        "Block",
        "process", &auv::vision::Block::process);
    state.new_usertype<auv::vision::SingleBlock>(
        "SingleBlock",
        "start", &auv::vision::SingleBlock::start,
        "stop", &auv::vision::SingleBlock::stop,
        "get_result", &auv::vision::SingleBlock::get_result);
    state.new_usertype<auv::vision::ThresholdBlock<int>>(
        "ThresholdBlock",
        sol::base_classes, sol::bases<auv::vision::SingleBlock>(),
        "set_params", &auv::vision::ThresholdBlock<int>::set_params);
    state.new_usertype<auv::vision::Camera>(
        "Camera",
        sol::call_constructor, sol::constructors<auv::vision::Camera(int, double, double, double, double, double, double, double, double, double)>(),
        "get_frame", &auv::vision::Camera::get_frame);

    state.new_usertype<auv::vision::AxisResult>(
        "AxisResult",
        "x", &auv::vision::AxisResult::x,
        "y", &auv::vision::AxisResult::y,
        "z", &auv::vision::AxisResult::z,
        "rot", &auv::vision::AxisResult::rot);
    state.new_usertype<auv::vision::AlgorithmResult>(
        "AlgorithmResult",
        "axis", &auv::vision::AlgorithmResult::axis,
        "frame", &auv::vision::AlgorithmResult::frame);
    state.new_usertype<auv::vision::RedBarBlock>(
        "VisionRedBar",
        sol::base_classes, sol::bases<auv::vision::ThresholdBlock<int>>(),
        "process", &auv::vision::RedBarBlock::process,
        "start", &auv::vision::RedBarBlock::start,
        "stop", &auv::vision::RedBarBlock::stop,
        "get_result", &auv::vision::RedBarBlock::get_result);
    state.set_function("imshow", [](const cv::Mat &frame) {
      cv::imshow("pre", frame);
      cv::waitKey(1);
    });
  });

  app.add_command("/run", [&app](const std::vector<std::string> splits) {
    if (splits.size() < 2) return;

    std::string file_name = splits[1];
    std::ifstream ifs(file_name);
    if(!ifs.is_open()) {
      std::cout << "Could not open the lua code file." << std::endl;
      return;
    }

    std::string str;
    std::string line;
    while (std::getline(ifs, line)) {
      str += line;
    }
    try {
      app.script(str);
    } catch(...) { }
  });

  //auv::vision::RedBarBlock b;
  //auv::vision::Camera cam(0, 588.4306598875787, 322.7472860229715, 592.781786987308, 242.4471017083893, -0.1443039341764572, 0.91856728920134, 0.0, 0.0, -2.402839834767997);
  //b.start(cam);
  //  while (true) {
  //    cv::imshow("pre", b.get_result().frame);
  //    cv::waitKey(1);
  //  }
  app.run();
  return 0;
}
