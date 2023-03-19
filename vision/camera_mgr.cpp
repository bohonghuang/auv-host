#include "camera_mgr.h"

#include "common.h"

namespace auv::vision {


CameraManager &CameraManager::GetInstance() noexcept {
  static CameraManager cameramgr;
  return cameramgr;
}


void CameraManager::add_capture(const std::function<std::vector<cv::VideoCapture>()> &init_func) noexcept {
  for (const auto &i: init_func()) {
    m_video_captures.push_back(i);
  }
}


void CameraManager::add_capture(const std::vector<CaptureParams> &indexs) noexcept {
  for (const auto &i: indexs) {
    std::stringstream ss;
    ss << "v4l2src device=/dev/video"
       << i.index
       << " ! video/x-raw, format=(string)YUY2, width=(int)"
       << i.size.width
       << ", height=(int)"
       << i.size.height
       << " ! videoconvert ! appsink ";
    auto cap = cv::VideoCapture(ss.str());
    m_video_captures.push_back(cap);
  }
}


cv::VideoCapture &CameraManager::get_capture(int index) noexcept {
  if (index >= m_video_captures.size())
    ASSERT(false, "The index is out of the video vector size!");
  return m_video_captures[index];
}


}// namespace auv::vision
