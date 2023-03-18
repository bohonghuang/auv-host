#include "camera.h"

#include "common.h"

namespace auv::vision {

void CameraBlock::SetFrameRate(float val) {
  s_frame_rate = val;
}

float CameraBlock::GetFrameRate() {
  return s_frame_rate;
}

CameraBlock::CameraBlock(int index) noexcept {
  m_capture.open("v4l2src device=/dev/video0 ! video/x-raw, format=(string)YUY2, width=(int)640, height=(int)480 ! videoconvert ! appsink ");
  ASSERT(m_capture.isOpened(), std::to_string(index) + ": camera open error.")

  std::string back_end = m_capture.getBackendName();
  int width = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_WIDTH));
  int height = static_cast<int>(m_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
  std::cout << "Open \"" << index << "\" camera success, it will use " << back_end
            << " as backend. width = " << width << "  height = " << height << std::endl;
}

CameraBlock::CameraBlock(const std::string &path) noexcept {
  m_capture.open(path);
}

cv::Mat CameraBlock::process(auv::unit_t) noexcept {
  cv::Mat frame;
  if (!m_capture.read(frame)) {
    return m_last_frame;
  }

  m_last_frame = frame;
  return frame;
}


}// namespace auv::vision
