#include "camera_mgr.h"

#include "common.h"

namespace auv::vision {


CameraManager &CameraManager::GetInstance() noexcept {
  static CameraManager cameramgr;
  return cameramgr;
}


void CameraManager::add_capture(std::function<std::vector<cv::VideoCapture>()> init_func) noexcept {
  for (const auto &i: init_func()) {
    m_video_captures.push_back(i);
  }
}


void CameraManager::add_capture(const std::vector<CaptureParams> &indexs) noexcept {
  for (const auto &i: indexs) {
    auto cap = cv::VideoCapture(i.index);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, i.size.width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, i.size.height);
    m_video_captures.push_back(cap);
  }
}


cv::VideoCapture &CameraManager::get_capture(int index) noexcept {
  if (index >= m_video_captures.size())
    ASSERT(false, "The index is out of the video vector size!");
  return m_video_captures[index];
}


}// namespace auv::vision
