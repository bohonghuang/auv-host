#include "camera.h"

#include "common.h"

namespace auv::vision {


void CameraBlock::SetFrameRate(float val) {
  s_frame_rate = val;
}


float CameraBlock::GetFrameRate() {
  return s_frame_rate;
}


CameraBlock::CameraBlock(cv::VideoCapture &cap) noexcept
    : m_capture(cap) {
  //  m_capture.open("v4l2src device=/dev/video0 ! video/x-raw, format=(string)YUY2, width=(int)640, height=(int)480 ! videoconvert ! appsink ");
  //  ASSERT(m_capture.isOpened(), std::to_string(index) + ": camera open error.")

  std::string back_end = m_capture.getBackendName();
  int width = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_WIDTH));
  int height = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
  std::cout << "Open camera success, it will use " << back_end
            << " as backend. width = " << width << "  height = " << height << std::endl;
}


static std::mutex s_mutex;

cv::Mat CameraBlock::process(auv::unit_t) noexcept {
  cv::Mat frame;

  std::unique_lock<std::mutex> lock(s_mutex);
  if (!m_capture.read(frame)) {
    return m_last_frame;
  }
  lock.unlock();

  m_last_frame = frame;
  return frame;
}


}// namespace auv::vision
