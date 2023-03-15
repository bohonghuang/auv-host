//
// Created by Qff on 2023/3/3.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>

#include "../runtime/block.h"

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

class CameraBlock : public auv::Block<std::tuple<>, cv::Mat> {
public:
  static void SetFrameRate(float val);
  static float GetFrameRate();

public:
  CameraBlock() noexcept = default;
  CameraBlock(int index, const CameraParams &camera_params) noexcept;
  CameraBlock(int index, double fx, double cx, double fy, double cy, double k1, double k2, double k3, double k4, double k5) noexcept;
  CameraBlock(std::string_view path, const CameraParams &camera_params) noexcept;

  cv::Mat process(In) noexcept override;

private:
  cv::VideoCapture m_capture;
  cv::Size m_frame_size;
  cv::Mat m_last_frame;
  CameraParams m_camera_params{};
  inline static float s_frame_rate = 30.0f;
};

}// namespace auv::vision

#endif//CAMERA_H
