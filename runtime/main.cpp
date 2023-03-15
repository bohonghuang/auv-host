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
    // auv::AddVisionBlock<auv::vision::RedBarBlock>(state, "VisionRedBar");
  });

  app.add_command("/run", [&app](const std::vector<std::string>& splits) {
    if (splits.size() < 2) return;

    const std::string& file_name = splits[1];
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
