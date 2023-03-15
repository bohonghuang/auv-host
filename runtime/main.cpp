//
// Created by Qff on 2023/2/27.
//

#include <fstream>

#include "../vision/red_bar.h"
#include "application.h"

int main() {
  auv::Application app([](sol::state &state) {
    auv::AddVisionBlock<auv::vision::RedBarBlock>(state, "VisionRedBar");
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

  app.run();
}
