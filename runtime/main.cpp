//
// Created by Qff on 2023/2/27.
//

#include <fstream>

#include "application.h"
#include "../vision/camera.h"
#include "../vision/block/inrange.h"
#include "../vision/block/find_bar.h"

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
  auv::vision::InRangeBlock block{auv::vision::InRangeBlock::ColorType::HLS, 0,0,0,0,0,0};
  auv::vision::FindBarBlock find_bar;
  auto out = camera | block | find_bar;
  auto res = out.process({});
  cv::Mat result;
  cv::imshow("preview", std::get<0>(res));
  cv::waitKey();
  app.run();
}
