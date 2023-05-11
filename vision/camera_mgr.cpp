#include "camera_mgr.h"

#include "common.h"

namespace auv::vision {


CameraManager &CameraManager::GetInstance() noexcept {
  static CameraManager cameramgr;
  return cameramgr;
}


void CameraManager::add_capture(const std::vector<CaptureParams> &indexs) noexcept {
  for (const auto &i: indexs) {
    auto it = m_video_captures.find(i.index);
    if (it != m_video_captures.end())
      return;

    if (i.index.size() <= 3) {
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
    } else {
      auto cap = cv::VideoCapture(i.index);
      m_video_captures[i.index] = cap;
    }
  }
}


cv::VideoCapture &CameraManager::get_capture(const std::string &index, int width, int height) noexcept {
  std::unique_lock<std::mutex> lock(m_mutex);
  auto it = m_video_captures.find(index);
  if (it == m_video_captures.end()) {
    std::cout << "capture index : " << index << "  " << width << " " << height << std::endl;
    this->add_capture({{index, {width, height}}});
    return m_video_captures[index];
  }

  if (!m_video_captures[index].isOpened())
    ASSERT(false, "The Capture is not opened! Please check the index!")
  return m_video_captures[index];
}


}// namespace auv::vision
