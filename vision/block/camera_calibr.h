#ifndef AUV_HOST_CAMERA_CALIBR_H
#define AUV_HOST_CAMERA_CALIBR_H

#include <opencv2/opencv.hpp>
#include <sol/sol.hpp>

#include "block.h"

namespace auv::vision {

struct CameraParams {
  double fx;
  double cx;
  double fy;
  double cy;
  double k1;
  double k2;
  double k3;
  double k4;
  double k5;
};

class CameraCalibrateBlock : public auv::Block<cv::Mat, cv::Mat> {
public:
  CameraCalibrateBlock() = default;
  explicit CameraCalibrateBlock(const CameraParams &camera_params) noexcept;
  explicit CameraCalibrateBlock(const sol::table &camera_params) noexcept;
  Out process(In) noexcept override;
  AUV_BLOCK;

private:
  CameraParams m_camera_params{};
};


}// namespace auv::vision

#endif//AUV_HOST_CAMERA_CALIBR_H
