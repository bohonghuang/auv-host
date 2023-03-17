//
// Created by Qff on 2023/3/3.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>

#include "block.h"

namespace auv::vision {

class CameraBlock : public auv::Block<std::tuple<>, cv::Mat> {
public:
  static void SetFrameRate(float val);
  static float GetFrameRate();

public:
  CameraBlock() noexcept = default;
  explicit CameraBlock(int index) noexcept;
  explicit CameraBlock(const std::string& path) noexcept;

  Out process(In) noexcept override;
  AUV_BLOCK;
private:
  cv::VideoCapture m_capture;
  cv::Mat m_last_frame;
  inline static float s_frame_rate = 30.0f;
};

}// namespace auv::vision

#endif//CAMERA_H
