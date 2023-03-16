//
// Created by qff233 on 23-3-16.
//

#ifndef AUV_HOST_BIOLOGY_H
#define AUV_HOST_BIOLOGY_H

#include "block.h"

#include <opencv2/opencv.hpp>

#include "nn/yolo_fast_v2.h"

namespace auv::vision {

class FindBiologyBlock : public Block<cv::Mat, std::map<std::string, std::vector<network::YoloFastV2Result>>> {
public:
  explicit FindBiologyBlock(const std::string& data_path);

  Out process(const In&) override;
private:
  network::YoloFastV2 m_yolo;
};

}

#endif//AUV_HOST_BIOLOGY_H
