#include "camera_mgr.h"

#include "common.h"

namespace auv::vision {

CameraManager::CameraManager(std::function<std::vector<cv::VideoCapture>()> init_func) {
  m_video_captures = std::move(init_func());
}


CameraManager::CameraManager(const std::vector<CameraData>& indexs) noexcept {
  m_video_captures.resize(indexs.size());
  for (size_t i = 0; i < indexs.size(); ++i) {
    auto param = indexs[i];
    auto cap = cv::VideoCapture(param.index);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, param.size.width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, param.size.height);

    m_video_captures[i] = cap;
  }
}


cv::VideoCapture& CameraManager::get_capture(int index) noexcept {
  if (index >= m_video_captures.size())
    ASSERT(false, "The index is out of the video vector size!");
  return m_video_captures[index];
}


}// namespace auv::vision
