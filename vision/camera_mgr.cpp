#include "camera_mgr.h"

#include "common.h"

namespace auv::vision {


CameraManager &CameraManager::GetInstance() noexcept {
  static CameraManager cameramgr;
  return cameramgr;
}


void CameraManager::add_capture(const std::vector<CaptureParams> &indexs) noexcept {
  for (const auto &i: indexs) {
    if (i.index < m_video_captures.size())
      continue;
    m_video_captures.resize(i.index + 1);
    std::stringstream ss;
    ss << "v4l2src device=/dev/video"
       << i.index
       << " ! video/x-raw, format=(string)YUY2, width=(int)"
       << i.size.width
       << ", height=(int)"
       << i.size.height
       << " ! videoconvert ! appsink ";
    auto cap = cv::VideoCapture(ss.str());
    m_video_captures[i.index] = cap;
  }
}


cv::VideoCapture &CameraManager::get_capture(int index) noexcept {
  if (index >= m_video_captures.size()) {
    this->add_capture({{index, {640, 480}}});
    return m_video_captures[index];
  }

  if (!m_video_captures[index].isOpened())
    ASSERT(false, "The Capture is not opened! Please check the index!")
  return m_video_captures[index];
}


}// namespace auv::vision
