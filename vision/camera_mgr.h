#ifndef AUV_HOST_CAMERA_MGR_H
#define AUV_HOST_CAMERA_MGR_H

#include <opencv2/opencv.hpp>
#include <mutex>

namespace auv::vision {

struct CaptureParams {
  std::string index;
  cv::Size size;
};

class CameraManager {
public:
  static CameraManager &GetInstance() noexcept;

  void add_capture(const std::vector<CaptureParams> &indexs) noexcept;

  cv::VideoCapture &get_capture(const std::string& file) noexcept;
private:
  CameraManager() = default;

private:
  std::map<std::string, cv::VideoCapture> m_video_captures;
  std::mutex m_mutex;
};

}// namespace auv::vision

#endif//AUV_HOST_CAMERA_MGR_H
