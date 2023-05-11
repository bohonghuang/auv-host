#include "camera_calibr.h"

namespace auv::vision {

CameraCalibrateBlock::CameraCalibrateBlock(const CameraParams &camera_params) noexcept
    : m_camera_params(camera_params) {}


cv::Mat
CameraCalibrateBlock::process(cv::Mat frame) noexcept {
  if (!m_has_init)
    m_distort_map = [this, &frame]() -> std::array<cv::Mat, 2> {
      cv::Mat map[2];
      cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << m_camera_params.fx,
                               0, m_camera_params.cx,
                               0, m_camera_params.fy, m_camera_params.cy,
                               0, 0, 1);
      cv::Mat distort_coeffs = (cv::Mat_<double>(5, 1) << m_camera_params.k1, m_camera_params.k2,
                                m_camera_params.k3, m_camera_params.k4,
                                m_camera_params.k5);
      cv::initUndistortRectifyMap(camera_matrix, distort_coeffs,
                                  cv::Mat::eye(3, 3, CV_32F), camera_matrix,
                                  frame.size(), CV_32FC1, map[0], map[1]);
      return {map[0], map[1]};
    }();

  if (m_distort_map[0].empty())
    return frame;
  cv::remap(frame, frame, m_distort_map[0], m_distort_map[1], cv::INTER_LINEAR);
  return frame;
}

CameraCalibrateBlock::CameraCalibrateBlock(const sol::table &lua_table) noexcept {
#define XX(name)                                              \
  auto name = lua_table[#name];                               \
  if (!name.valid()) {                                        \
    std::cout << "camera_params need " << #name << std::endl; \
    return;                                                   \
  }

  XX(fx)
  XX(cx)
  XX(fy)
  XX(cy)
  XX(k1)
  XX(k2)
  XX(k3)
  XX(k4)
  XX(k5)
#undef XX
  m_camera_params = CameraParams{fx, cx, fy, cy, k1, k2, k3, k4, k5};
}


}// namespace auv::vision
