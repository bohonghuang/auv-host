//
// Created by qff233 on 23-3-17.
//

#ifndef AUV_HOST_IMSHOW_H
#define AUV_HOST_IMSHOW_H

#include <opencv2/opencv.hpp>

#include "block.h"

namespace auv::vision {

class ImshowBlock : public Block<cv::Mat, auv::unit_t> {
public:
  Out process(In) noexcept override;
  AUV_BLOCK;
};

}

#endif//AUV_HOST_IMSHOW_H
