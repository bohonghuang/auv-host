//
// Created by Qff on 2023/2/27.
//

#include <fstream>

#include "../vision/block/camera.h"
#include "../vision/block/find_bar.h"
#include "../vision/block/inrange.h"
#include "application.h"

int main() {
  auv::Application app([](sol::state &state) {
    state.new_enum<auv::vision::InRangeBlock::ColorType>(
        "ColorType",
        {
            {"YCrCb", auv::vision::InRangeBlock::ColorType::YCrCb},
            {"HLS", auv::vision::InRangeBlock::ColorType::HLS},
            {"HSV", auv::vision::InRangeBlock::ColorType::HSV},
        });

    state.new_usertype<auv::vision::CameraBlock>(
        "CameraBlock",
        sol::constructors<auv::vision::CameraBlock(int, double, double, double, double, double, double, double, double, double)>(),
        "process", &auv::vision::CameraBlock::process);

    state.new_usertype<auv::vision::InRangeBlock>(
        "InRangeBlock",
        sol::constructors<auv::vision::InRangeBlock(auv::vision::InRangeBlock::ColorType, int, int, int, int, int, int)>(),
        "process", &auv::vision::InRangeBlock::process);

    state.new_usertype<auv::vision::FindBarResult>(
        "FindBarResult",
        "short_side_cent_point", &auv::vision::FindBarResult::short_side_cent_point,
        "long_side_rot", &auv::vision::FindBarResult::long_side_rot);

    state.new_usertype<auv::vision::FindBarBlock>(
        "FindBarBlock",
        sol::constructors<auv::vision::FindBarBlock(bool)>(),
        "process", &auv::vision::FindBarBlock::process);
  });

  app.add_command("/run", [&app](const std::vector<std::string> &splits) {
    if (splits.size() < 2) return;

    const std::string &file_name = splits[1];
    std::ifstream ifs(file_name);
    if (!ifs.is_open()) {
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
    } catch (...) {}
  });

    auv::vision::CameraBlock camera(0, 588.4306598875787, 322.7472860229715, 592.781786987308, 242.4471017083893, -0.1443039341764572, 0.91856728920134, 0.0, 0.0, -2.402839834767997);
    auv::vision::InRangeBlock in_range{auv::vision::InRangeBlock::ColorType::YCrCb, 33,146,65,177,255,130};
    auv::vision::FindBarBlock find_bar(true);
    auto out = camera | in_range | find_bar;

    while(true) {
      auto res = out.process({});
      cv::imshow("preview", std::get<0>(res));
      auto result = std::get<1>(res);
      cv::waitKey();
    }

  app.run();
}
