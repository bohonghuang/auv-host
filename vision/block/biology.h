#ifndef AUV_HOST_BIOLOGY_H
#define AUV_HOST_BIOLOGY_H

#include "block.h"

#include <opencv2/opencv.hpp>

#include "nn/yolo_fast_v2.h"

namespace auv::vision {

struct FindBiologyBlockResults {
  cv::Mat frame;
  std::map<std::string, std::vector<network::YoloFastV2Result>> result;
};

class FindBiologyBlock : public Block<cv::Mat, FindBiologyBlockResults> {
public:
  explicit FindBiologyBlock(const std::string &data_path = "./model_data/marine.data");
  Out process(In) override;
  AUV_BLOCK;

private:
  network::YoloFastV2 m_yolo;
};

}// namespace auv::vision

#endif//AUV_HOST_BIOLOGY_H
