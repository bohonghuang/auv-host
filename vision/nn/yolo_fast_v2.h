#ifndef NEURAL_NETWORK_YOLO_FAST_V2_
#define NEURAL_NETWORK_YOLO_FAST_V2_

#include <string>
#include <vector>
#include <array>
#include <opencv2/opencv.hpp>

namespace auv::vision::network {

struct YoloFastV2Result {
    std::string name;
    cv::Rect rect;
    float confidences;
};

struct YoloFastV2ModelConfig {
    int epochs          = 501;
    int batch_size      = 32;
    int subdivisions    = 1;
    int classes         = 5;
    int width           = 352;
    int height          = 352;
    int anchor_num      = 3;
    float learning_rate = 0.001;
    std::string val_path;
    std::string model_name;
    std::string train_path;
    std::string names_path;
    std::string pre_weights;
    std::vector<float> anchors;
    std::vector<int> steps = {150, 200, 300, 400};
};

class YoloFastV2 {
public:
  explicit YoloFastV2(const std::string& module_config_file_path, float obj_threshold=0.3,
               float conf_threshold=0.3, float nms_threshold=0.4);

  cv::Mat forward(const cv::Mat& src_img);
  std::map<std::string, std::vector<YoloFastV2Result>> process(const cv::Mat& img, const cv::Mat& detect_result);
private:
  void load_data_file(const std::string& file_path);
  void draw_pred(cv::Mat& img, float confidence, int class_id, const cv::Rect& rect_range);
private:
  YoloFastV2ModelConfig m_model_config;
  std::map<std::string, std::string> m_configs;
  std::vector<std::string> m_classes;
  cv::Mat m_anchors;
  float m_obj_threshold;
  float m_conf_threshold;
  float m_nms_threshold;
  std::array<int, 2> m_stride = {16, 32};
  cv::dnn::Net m_net;
};

} // namespace auv::vision::network


#endif