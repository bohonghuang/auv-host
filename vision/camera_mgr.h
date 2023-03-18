#ifndef AUV_HOST_CAMERA_MGR_H
#define AUV_HOST_CAMERA_MGR_H

#include <opencv2/opencv.hpp>

namespace auv::vision {

struct CameraData {
  int index;
  cv::Size size;
};

class CameraManager {
public:
  explicit CameraManager(std::function<std::vector<cv::VideoCapture>()> init_func);
  explicit CameraManager(const std::vector<CameraData>& indexs) noexcept;

  cv::VideoCapture& get_capture(int index) noexcept;
private:
  std::vector<cv::VideoCapture> m_video_captures;
};

}

#endif//AUV_HOST_CAMERA_MGR_H
