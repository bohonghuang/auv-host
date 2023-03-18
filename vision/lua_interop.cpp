#include "lua_interop.h"
#include "runtime/lua_interop.h"

#include "block/camera.h"
#include "block/camera_calibr.h"
#include "block/color.h"
#include "block/find_bar.h"

void auv::vision::lua::setup_env(sol::state &state) {
  if (state["vision"].is<sol::table>()) return;
  auv::lua::setup_env(state);
  state.create_named_table("vision");
  state.new_usertype<cv::Point>(
      "Point",
      "x", &cv::Point::x,
      "y", &cv::Point::y);

  state.set_function("imshow", [](const cv::Mat &frame) {
    cv::imshow("pre", frame);
    cv::waitKey(1);
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
}
