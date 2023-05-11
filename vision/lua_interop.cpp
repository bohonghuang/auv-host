#include "lua_interop.h"
#include "runtime/lua_interop.h"

#include "block/camera.h"
#include "block/camera_calibr.h"
#include "block/color.h"
#include "block/detect.h"
#include "block/find_ball.h"
#include "block/find_bar.h"
#include "block/find_door.h"
#include "block/find_door_grid.h"
#include "block/imshow.h"

#include "camera_mgr.h"
#include <sol/raii.hpp>

void auv::vision::lua::setup_env(sol::state &state) {
  if (state["vision"].is<sol::table>()) return;
  auv::lua::setup_env(state);
  state.create_named_table("vision");

  state.set_function("GetCaptureManager", []() -> auv::vision::CameraManager & {
    return auv::vision::CameraManager::GetInstance();
  });

  state.set_function("GetCapture", sol::overload([](const std::string &index, int width, int height) -> cv::VideoCapture & {
                       return auv::vision::CameraManager::GetInstance().get_capture(index, width, height);
                     }));

  AUV_NEW_SOL_TYPE(state, auv::vision::UploadBlock, sol::constructors<auv::vision::UploadBlock(const std::string &, int, int)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::ImshowBlock, sol::default_constructor);

  AUV_NEW_SOL_TYPE(state, auv::vision::CameraBlock,
                   sol::constructors<auv::vision::CameraBlock(cv::VideoCapture &)>());

  //  AUV_NEW_SOL_TYPE(state, auv::vision::CameraCalibrateBlock,
  //                   sol::constructors<auv::vision::CameraCalibrateBlock(const auv::vision::CameraParams &)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::CameraCalibrateBlock,
                   sol::constructors<auv::vision::CameraCalibrateBlock(const sol::table &)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::InRangeBlock,
                   sol::constructors<auv::vision::InRangeBlock(const sol::table &)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::ConvertColorBlock,
                   sol::constructors<auv::vision::ConvertColorBlock(int)>());

  sol::table ns_cv = state.create_named_table("cv");
  ns_cv["COLOR_BGR2HSV"] = cv::COLOR_BGR2HSV;
  ns_cv["COLOR_BGR2YCrCb"] = cv::COLOR_BGR2YCrCb;
  ns_cv["COLOR_GRAY2BGR"] = cv::COLOR_GRAY2BGR;
  ns_cv["COLOR_RGB2HLS"] = cv::COLOR_RGB2HLS;
  ns_cv["COLOR_RGB2HSV"] = cv::COLOR_RGB2HSV;
  AUV_NEW_SOL_TYPE(ns_cv, cv::Mat, sol::default_constructor);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Point, sol::default_constructor, "x", &cv::Point::x, "y", &cv::Point::y);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Point2f, sol::default_constructor, "x", &cv::Point2f::x, "y", &cv::Point2f::y);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Point2d, sol::default_constructor, "x", &cv::Point2d::x, "y", &cv::Point2d::y);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Size, sol::default_constructor, "width", &cv::Size::width, "height", &cv::Size::height);
  AUV_NEW_SOL_TYPE(ns_cv, cv::Rect2f, sol::default_constructor, "width", &cv::Rect2f::width, "height", &cv::Rect2f::height, "x", &cv::Rect2f::x, "y", &cv::Rect2f::y);
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

  AUV_NEW_SOL_TYPE(state, auv::vision::FindDoorResult, sol::no_constructor,
                   "left", &auv::vision::FindDoorResult::left,
                   "right", &auv::vision::FindDoorResult::right,
                   "bottom", &auv::vision::FindDoorResult::bottom);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindDoorResults, sol::no_constructor,
                   "frame", &auv::vision::FindDoorResults::frame,
                   "result", &auv::vision::FindDoorResults::result);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindDoorGridResults, sol::no_constructor,
                   "frame", &auv::vision::FindDoorGridResults::frame,
                   "mat", &auv::vision::FindDoorGridResults::mat);

  AUV_NEW_SOL_TYPE(state, auv::vision::FindDoorGrid,
                   sol::constructors<auv::vision::FindDoorGrid(int, int)>());

  AUV_NEW_SOL_TYPE(state, auv::vision::FindLineBlock,
                   sol::constructors<auv::vision::FindLineBlock(double, double, int, bool)>());

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
