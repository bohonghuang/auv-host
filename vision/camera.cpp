//
// Created by Qff on 2023/3/3.
//

#include "camera.h"

#include "common.h"

namespace auv::vision {


Camera::Camera(int index, const CameraParams &camera_params) noexcept
    : m_camera_params(camera_params) {
  m_capture.open(index);
  ASSERT(m_capture.isOpened(), std::to_string(index) + ": camera open error.")

  std::string back_end = m_capture.getBackendName();
  int width = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_WIDTH));
  int height = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
  m_frame_size = {width, height};
  std::cout << "Open \"1\" camera success, it will use " << back_end
            << " as backend. width = " << width << "  height = " << height << std::endl;
}

Camera::Camera(int index, double fx, double cx, double fy, double cy, double k1, double k2, double k3, double k4, double k5) noexcept {
  m_camera_params = {fx, cx, fy, cy, k1, k2, k3, k4, k5};
  m_capture.open("v4l2src device=/dev/video0 ! video/x-raw, format=(string)YUY2, width=(int)640, height=(int)480 ! videoconvert ! appsink ");
  ASSERT(m_capture.isOpened(), std::to_string(index) + ": camera open error.")

  std::string back_end = m_capture.getBackendName();
  int width = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_WIDTH));
  int height = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
  m_frame_size = {width, height};
  std::cout << "Open \"" << index << "\" camera success, it will use " << back_end
            << " as backend. width = " << width << "  height = " << height << std::endl;
}

Camera::Camera(std::string_view path, const CameraParams &camera_params) noexcept
    : m_camera_params(camera_params) {
  m_capture.open(path.data());
}

cv::Mat Camera::get_frame() noexcept {
  static const auto distort_map = [this]() -> std::array<cv::Mat, 2> {
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
                                m_frame_size, CV_32FC1, map[0], map[1]);
    return {map[0], map[1]};
  }();

  cv::Mat frame;
  if (!m_capture.read(frame)) {
    return m_last_frame;
  }

  cv::remap(frame, frame, distort_map[0], distort_map[1], cv::INTER_LINEAR);
  m_last_frame = frame;
  return frame;
}



}// namespace auv::vision