#include "imshow.h"

#include <utility>

namespace auv::vision {


cv::Mat
ImshowBlock::process(cv::Mat frame) noexcept {
  cv::imshow("preview", frame);
  cv::waitKey(1);
  return frame;
}


UploadBlock::UploadBlock(std::string address, int width, int height)
    : m_address(std::move(address)), width(width), height(height) {
}


cv::Mat
UploadBlock::process(cv::Mat frame) noexcept {
  static cv::VideoWriter writer = [this]() -> cv::VideoWriter {
    std::cout << m_address << std::endl;
    cv::VideoWriter writer(m_address, 0, 30.0, cv::Size(width, height));
    return writer;
  }();

  if (writer.isOpened() && width == frame.size().width && height == frame.size().height)
    writer.write(frame.clone());
  else
    std::cout << "VideoWriter don't open, or the frame size is error!"
              << "frame width : " << frame.size().width << "   height : " << frame.size().height
              << "   needed frame width : " << width << "   height" << height << std::endl;
  return frame;
}


}// namespace auv::vision
