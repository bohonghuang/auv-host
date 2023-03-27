#include "lua_interop.h"
#include "runtime/lua_interop.h"

#include "block/camera.h"
#include "block/camera_calibr.h"
#include "block/color.h"
#include "block/detect.h"
#include "block/find_ball.h"
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

  state.set_function("GetCapture", sol::overload([](const std::string &index) -> cv::VideoCapture & {
                       return auv::vision::CameraManager::GetInstance().get_capture(index);
                     }));

  AUV_NEW_SOL_TYPE(state, auv::vision::UploadBlock, sol::constructors<auv::vision::UploadBlock(const std::string &, int, int)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::ImshowBlock, sol::default_constructor);

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
                   sol::constructors<auv::vision::CameraBlock(cv::VideoCapture &)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::CameraCalibrateBlock,
                   sol::constructors<auv::vision::CameraCalibrateBlock(const auv::vision::CameraParams &)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::InRangeParams, sol::default_constructor,
                   "low_1", &auv::vision::InRangeParams::low_1,
                   "high_1", &auv::vision::InRangeParams::high_1,
                   "low_2", &auv::vision::InRangeParams::low_2,
                   "high_2", &auv::vision::InRangeParams::high_2,
                   "low_3", &auv::vision::InRangeParams::low_3,
                   "high_3", &auv::vision::InRangeParams::high_3);

  AUV_NEW_SOL_TYPE(state, auv::vision::InRangeBlock,
                   sol::constructors<auv::vision::InRangeBlock(const sol::table &)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::ConvertColorBlock,
                   sol::constructors<auv::vision::ConvertColorBlock(int)>());

  sol::table ns_cv = state.create_named_table("cv");
  ns_cv["COLOR_BGR2HSV"] = cv::COLOR_BGR2HSV;
  ns_cv["COLOR_BGR2YCrCb"] = cv::COLOR_BGR2YCrCb;
  ns_cv["COLOR_GRAY2BGR"] = cv::COLOR_GRAY2BGR;
  AUV_NEW_SOL_TYPE(ns_cv, cv::Mat, sol::default_constructor);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Point, sol::default_constructor, "x", &cv::Point::x, "y", &cv::Point::y);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Point2d, sol::default_constructor, "x", &cv::Point2d::x, "y", &cv::Point2d::y);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Size, sol::default_constructor, "width", &cv::Size::width, "height", &cv::Size::height);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Rect, sol::default_constructor, "width", &cv::Rect::width, "height", &cv::Rect::height, "x", &cv::Rect::x, "y", &cv::Rect::y);
  ns_cv.set_function("imshow", sol::resolve<void(const std::string &, cv::InputArray)>(&cv::imshow));
  ns_cv.set_function("waitKey", sol::resolve<int(int)>(&cv::waitKey));
  ns_cv.set_function("destroyAllWindows", &cv::destroyAllWindows);
  AUV_NEW_SOL_TYPE(state, auv::vision::FindBarResults, sol::no_constructor,
                   "frame", &auv::vision::FindBarResults::frame,
                   "result", &auv::vision::FindBarResults::result);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindBarResult, sol::default_constructor,
                   "area", &auv::vision::FindBarResult::area,
                   "points", &auv::vision::FindBarResult::points);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindBarBlock,
                   sol::constructors<auv::vision::FindBarBlock(bool)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::FindBallResults, sol::no_constructor,
                   "frame", &auv::vision::FindBallResults::frame,
                   "result", &auv::vision::FindBallResults::result);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindBallResult, sol::default_constructor,
                   "points", &auv::vision::FindBallResult::points);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindBallBlock,
                   sol::constructors<auv::vision::FindBarBlock(bool)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::network::YoloFastV2Result, sol::no_constructor,
                   "name", &auv::vision::network::YoloFastV2Result::name,
                   "rect", &auv::vision::network::YoloFastV2Result::rect,
                   "confidence", &auv::vision::network::YoloFastV2Result::confidence);

  AUV_NEW_SOL_TYPE(state, auv::vision::ObjectDetectResult, sol::no_constructor,
                   "name", &auv::vision::ObjectDetectResult::name,
                   "x", &auv::vision::ObjectDetectResult::x,
                   "y", &auv::vision::ObjectDetectResult::y,
                   "width", &auv::vision::ObjectDetectResult::width,
                   "height", &auv::vision::ObjectDetectResult::height,
                   "confidence", &auv::vision::ObjectDetectResult::confidence);

  AUV_NEW_SOL_TYPE(state, auv::vision::ObjectDetectResults, sol::no_constructor,
                   "frame", &auv::vision::ObjectDetectResults::frame,
                   "result", &auv::vision::ObjectDetectResults::result);
  
  AUV_NEW_SOL_TYPE(state, auv::vision::ObjectDetectBlock,
                   sol::default_constructor);

  static bool initial_invocation = true;
  if (initial_invocation) {
    auv::lua::setup_env_all = auv::vision::lua::setup_env;
    initial_invocation = false;
  }
}
