#ifndef AUV_HOST_DETECT_H
#define AUV_HOST_DETECT_H

#include "block.h"

#include <opencv2/opencv.hpp>

#include "nn/yolo_fast_v2.h"

namespace auv::vision {

struct ObjectDetectResult{
  std::string name;
  double confidence;
  double x;
  double y;
  double width;
  double height;
};

struct ObjectDetectResults {
  cv::Mat frame;
  std::vector<ObjectDetectResult> result;
};

class ObjectDetectBlock : public Block<cv::Mat, ObjectDetectResults> {
public:
  explicit ObjectDetectBlock(const std::string &data_path = "./model_data/marine.data");
  Out process(In) override;
  AUV_BLOCK;

private:
  network::YoloFastV2 m_yolo;
};

}// namespace auv::vision

#endif//AUV_HOST_DETECT_H
