//
// Created by Qff on 2023/3/3.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>

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

class Camera {
public:
  Camera() noexcept = default;
  Camera(int index, const CameraParams &camera_params) noexcept;
  Camera(int index, double fx, double cx, double fy, double cy, double k1, double k2, double k3, double k4, double k5) noexcept;
  Camera(std::string_view path, const CameraParams &camera_params) noexcept;

  cv::Mat get_frame() noexcept;

private:
  cv::VideoCapture m_capture;
  cv::Size m_frame_size;
  cv::Mat m_last_frame;
  CameraParams m_camera_params{};
};

}// namespace auv::vision

#endif//CAMERA_H
