#include <catch2/catch_test_macros.hpp>

#include "block/biology.h"
#include "block/camera.h"
#include "block/camera_calibr.h"
#include "block/color.h"
#include "block/find_bar.h"
#include "block/imshow.h"
#include "camera_mgr.h"
#include "nn/model_lib.h"

#include <chrono>

TEST_CASE("视觉算法集成测试") {
  namespace vision = auv::vision;

  auto cam = vision::CameraBlock(vision::CameraManager::GetInstance().get_capture(0));
  auto cal = vision::CameraCalibrateBlock(
      {588.4306598875787,
       322.7472860229715,
       592.781786987308,
       242.4471017083893,
       -0.1443039341764572,
       0.91856728920134,
       0.0,
       0.0,
       -2.40283983476799});
  auto cvt = vision::ConvertColorBlock(cv::COLOR_BGR2YCrCb);
  auto range = vision::InRangeBlock({33, 146, 65, 177, 255, 130});
  auto find = vision::FindBarBlock(true);
  auto bio = vision::FindBiologyBlock();
  SECTION("FindBar 巡线算法") {
    auto pipe = cam | cal | cvt | range | find;
    auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < 10; ++i) {
      auto t1 = std::chrono::steady_clock::now();
      auto [frame, result] = pipe.process({});
      auto t2 = std::chrono::steady_clock::now();
      std::cout << "time(s):" << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl;
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "total time(s):" << std::chrono::duration_cast<std::chrono::duration<double>>(end - begin).count() << std::endl;
  }
  SECTION("YoloV4Fastest 生物识别") {
    auto pipe = cam | cal | bio;
    auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < 10; ++i) {
      auto t1 = std::chrono::steady_clock::now();
      auto [frame, result] = pipe.process({});
      auto t2 = std::chrono::steady_clock::now();
      std::cout << "time(s):" << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl;

      for (const auto &it: result) {
        const auto &vec = it.second;
        for (const auto &j: vec) {
          auv::vision::network::YoloFastV2::YoloFastV2::draw_pred(frame, j.confidences, j.name, j.rect);
        }
      }
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "total time(s):" << std::chrono::duration_cast<std::chrono::duration<double>>(end - begin).count() << std::endl;
  }
}
