#include "lua_interop.h"
#include "runtime/lua_interop.h"

#include "block/camera.h"
#include "block/camera_calibr.h"
#include "block/color.h"
#include "block/detect.h"
#include "block/find_bar.h"
#include "block/imshow.h"

#include "camera_mgr.h"

void auv::vision::lua::setup_env(sol::state &state) {
  if (state["vision"].is<sol::table>()) return;
  auv::lua::setup_env(state);
  state.create_named_table("vision");

  state.set_function("GetCaptureManager", []() -> auv::vision::CameraManager & {
    return auv::vision::CameraManager::GetInstance();
  });

  state.set_function("GetCapture", [](int index) -> cv::VideoCapture & {
    return auv::vision::CameraManager::GetInstance().get_capture(index);
  });

  state.new_usertype<auv::vision::UploadBlock>(
      "UploadBlock",
      sol::constructors<auv::vision::UploadBlock(const std::string &, int, int)>(),
      AUV_BLOCK_SOL_METHODS(auv::vision::UploadBlock));

  AUV_NEW_SOL_TYPE(state, auv::vision::ImshowBlock, sol::default_constructor,
                   AUV_BLOCK_SOL_METHODS(auv::vision::ImshowBlock));

  AUV_NEW_SOL_TYPE(state, auv::vision::CameraParams, sol::default_constructor,
                   "fx", &auv::vision::CameraParams::fx,
                   "cx", &auv::vision::CameraParams::cx,
                   "fy", &auv::vision::CameraParams::fy,
                   "cy", &auv::vision::CameraParams::cy,
                   "k1", &auv::vision::CameraParams::k1,
                   "k2", &auv::vision::CameraParams::k2,
                   "k3", &auv::vision::CameraParams::k3,
                   "k4", &auv::vision::CameraParams::k4,
                   "k5", &auv::vision::CameraParams::k5);

  AUV_NEW_SOL_TYPE(state, auv::vision::CameraBlock,
                   sol::constructors<auv::vision::CameraBlock(cv::VideoCapture &)>(),
                   AUV_BLOCK_SOL_METHODS(auv::vision::CameraBlock));

  AUV_NEW_SOL_TYPE(state, auv::vision::CameraCalibrateBlock,
                   sol::constructors<auv::vision::CameraCalibrateBlock(const auv::vision::CameraParams &)>(),
                   AUV_BLOCK_SOL_METHODS(auv::vision::CameraCalibrateBlock));

  AUV_NEW_SOL_TYPE(state, auv::vision::InRangeParams, sol::default_constructor,
                   "low_1", &auv::vision::InRangeParams::low_1,
                   "high_1", &auv::vision::InRangeParams::high_1,
                   "low_2", &auv::vision::InRangeParams::low_2,
                   "high_2", &auv::vision::InRangeParams::high_2,
                   "low_3", &auv::vision::InRangeParams::low_3,
                   "high_3", &auv::vision::InRangeParams::high_3);

  AUV_NEW_SOL_TYPE(state, auv::vision::InRangeBlock, sol::constructors<auv::vision::InRangeBlock(), auv::vision::InRangeBlock(auv::vision::InRangeParams)>(),
                   AUV_BLOCK_SOL_METHODS(auv::vision::InRangeBlock));

  AUV_NEW_SOL_TYPE(state, auv::vision::ConvertColorBlock,
                   sol::constructors<auv::vision::ConvertColorBlock(int)>(),
                   AUV_BLOCK_SOL_METHODS(auv::vision::ConvertColorBlock));

  sol::table ns_cv = state.create_named_table("cv");
  ns_cv["COLOR_BGR2HSV"] = cv::COLOR_BGR2HSV;
  ns_cv["COLOR_BGR2YCrCb"] = cv::COLOR_BGR2YCrCb;
  ns_cv["COLOR_GRAY2BGR"] = cv::COLOR_GRAY2BGR;
  AUV_NEW_SOL_TYPE(ns_cv, cv::Mat, sol::default_constructor);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Point, sol::default_constructor, "x", &cv::Point::x, "y", &cv::Point::y);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Size, sol::default_constructor, "width", &cv::Size::width, "height", &cv::Size::height);
  ns_cv.set_function("imshow", sol::resolve<void(const std::string&, cv::InputArray)>(&cv::imshow));
  ns_cv.set_function("waitKey", sol::resolve<int(int)>(&cv::waitKey));
  ns_cv.set_function("destroyAllWindows", &cv::destroyAllWindows);
  AUV_NEW_SOL_TYPE(state, auv::vision::FindBarResults, sol::no_constructor,
                   "frame", &auv::vision::FindBarResults::frame,
                   "result", &auv::vision::FindBarResults::result);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindBarResult, sol::no_constructor,
                   "point", &auv::vision::FindBarResult::point,
                   "angle", &auv::vision::FindBarResult::angle);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindBarBlock,
                   sol::constructors<auv::vision::FindBarBlock(bool)>(),
                   AUV_BLOCK_SOL_METHODS(auv::vision::FindBarBlock));

  AUV_NEW_SOL_TYPE(state, auv::vision::ObjectDetectResults, sol::no_constructor,
                   "frame", &auv::vision::ObjectDetectResults::frame,
                   "result", &auv::vision::ObjectDetectResults::result);

  AUV_NEW_SOL_TYPE(state, auv::vision::ObjectDetectBlock,
                   sol::default_constructor,
                   AUV_BLOCK_SOL_METHODS(auv::vision::ObjectDetectBlock));

  static bool initial_invocation = true;
  if (initial_invocation) {
    auv::lua::setup_env_all = auv::vision::lua::setup_env;
    initial_invocation = false;
  }
}
