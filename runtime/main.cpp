//
// Created by Qff on 2023/2/27.
//

#include <fstream>

#include "../vision/block/camera.h"
#include "../vision/block/camera_calibr.h"
#include "../vision/block/color.h"
#include "../vision/block/find_bar.h"
#include "application.h"

#define AUV_BLOCK_SOL_METHODS(TYPE) "process", &TYPE::process, "as_untyped", &TYPE::as_untyped

int main() {
  auv::Application app([](sol::state &state) {
    state.new_usertype<auv::AnyBlock>(
        "AnyBlock", sol::no_constructor,
        "process", sol::overload(sol::resolve<std::any(std::any)>(&auv::AnyBlock::process), sol::resolve<std::any()>(&auv::AnyBlock::process)),
        "connect", &auv::AnyBlock::connect);

    state.set_function("connect", [](sol::variadic_args va) -> auv::AnyBlock {
      auto &&begin = *va.cbegin();
      static constexpr auto to_any_block = [](const sol::userdata &&block) -> auv::AnyBlock {
        return block.is<auv::AnyBlock>() ? block.as<auv::AnyBlock>() : block["as_untyped"].call<auv::AnyBlock>(block);
      };
      auto prev_block = to_any_block(begin);
      for (auto iter = va.cbegin() + 1; iter != va.cend(); iter++) {
        prev_block = prev_block.connect(to_any_block(*iter));
      }
      return prev_block;
    });

    /*-----------------------------------------------------------------------------------------------*/
    state.new_usertype<auv::vision::CameraParams>(
        "CameraParams",
        sol::default_constructor,
        "fx", &auv::vision::CameraParams::fx,
        "cx", &auv::vision::CameraParams::cx,
        "fy", &auv::vision::CameraParams::fy,
        "cy", &auv::vision::CameraParams::cy,
        "k1", &auv::vision::CameraParams::k1,
        "k2", &auv::vision::CameraParams::k2,
        "k3", &auv::vision::CameraParams::k3,
        "k4", &auv::vision::CameraParams::k4,
        "k5", &auv::vision::CameraParams::k5);

    state.new_usertype<auv::vision::CameraBlock>(
        "CameraBlock",
        sol::constructors<auv::vision::CameraBlock(int)>(),
        AUV_BLOCK_SOL_METHODS(auv::vision::CameraBlock));

    state.new_usertype<auv::vision::CameraCalibrateBlock>(
        "CameraCalibrateBlock",
        sol::constructors<auv::vision::CameraCalibrateBlock(const auv::vision::CameraParams &)>(),
        AUV_BLOCK_SOL_METHODS(auv::vision::CameraCalibrateBlock));

    /*-----------------------------------------------------------------------------------------------*/
    state.new_usertype<auv::vision::InRangeParams>(
        "InRangeParams", sol::default_constructor,
        "low_1", &auv::vision::InRangeParams::low_1,
        "high_1", &auv::vision::InRangeParams::high_1,
        "low_2", &auv::vision::InRangeParams::low_2,
        "high_2", &auv::vision::InRangeParams::high_2,
        "low_3", &auv::vision::InRangeParams::low_3,
        "high_3", &auv::vision::InRangeParams::high_3);

    state.new_usertype<auv::vision::InRangeBlock>(
        "InRangeBlock",
        sol::constructors<auv::vision::InRangeBlock(), auv::vision::InRangeBlock(auv::vision::InRangeParams)>(),
        AUV_BLOCK_SOL_METHODS(auv::vision::InRangeBlock));
    /*-----------------------------------------------------------------------------------------------*/

    state.new_usertype<auv::vision::ConvertColorBlock>(
        "ConvertColorBlock",
        sol::constructors<auv::vision::ConvertColorBlock(int)>(),
        AUV_BLOCK_SOL_METHODS(auv::vision::ConvertColorBlock));

    sol::table cv_namespace = state.create_named_table("cv");
    cv_namespace["COLOR_BGR2HSV"] = cv::COLOR_BGR2HSV;
    cv_namespace["COLOR_BGR2YCrCb"] = cv::COLOR_BGR2YCrCb;
    cv_namespace["COLOR_GRAY2BGR"] = cv::COLOR_GRAY2BGR;

    /*-----------------------------------------------------------------------------------------------*/
    state.new_usertype<auv::vision::FindBarResult>(
        "FindBarResult",
        "short_side_cent_point", &auv::vision::FindBarResult::short_side_cent_point,
        "long_side_rot", &auv::vision::FindBarResult::long_side_rot);

    state.new_usertype<auv::vision::FindBarBlock>(
        "FindBarBlock",
        sol::constructors<auv::vision::FindBarBlock(bool)>(),
        AUV_BLOCK_SOL_METHODS(auv::vision::FindBarBlock));
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
  app.run();
}
