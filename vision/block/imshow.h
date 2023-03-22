#ifndef AUV_HOST_IMSHOW_H
#define AUV_HOST_IMSHOW_H

#include <opencv2/opencv.hpp>

#include "block.h"

namespace auv::vision {

class ImshowBlock : public Block<cv::Mat, cv::Mat> {
public:
  Out process(In) noexcept override;
  AUV_BLOCK;
};

class UploadBlock : public Block<cv::Mat, cv::Mat> {
public:
  UploadBlock(std::string address, int width, int height);
  Out process(In) noexcept override;
  AUV_BLOCK;

private:
  std::string m_address;
  int width;
  int height;
};

}// namespace auv::vision

#endif//AUV_HOST_IMSHOW_H
