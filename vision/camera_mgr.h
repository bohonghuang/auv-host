#ifndef AUV_HOST_CAMERA_MGR_H
#define AUV_HOST_CAMERA_MGR_H

#include <opencv2/opencv.hpp>

namespace auv::vision {

struct CaptureParams {
  int index;
  cv::Size size;
};

class CameraManager {
public:
  static CameraManager& GetInstance() noexcept;

  void add_capture(std::function<std::vector<cv::VideoCapture>()> init_func) noexcept;
  void add_capture(const std::vector<CaptureParams> &indexs) noexcept;

  cv::VideoCapture &get_capture(int index) noexcept;
private:
  CameraManager() = default;
private:
  std::vector<cv::VideoCapture> m_video_captures;
};

}// namespace auv::vision

#endif//AUV_HOST_CAMERA_MGR_H
