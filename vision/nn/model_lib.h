#ifndef AUV_HOST_MODEL_LIB_H
#define AUV_HOST_MODEL_LIB_H

#include <map>
#include <opencv2/opencv.hpp>
#include <string>

namespace auv::vision::network {

enum class NetWorkAccType {
  CPU,
  GPU
};

struct ModelParams {
  NetWorkAccType acc_type = NetWorkAccType::GPU;
  cv::dnn::Net net;

  std::string file_name;
  bool is_loaded = false;
};

class ModelLibs {
public:
  static ModelLibs &GetInstance() noexcept;

  void add_model(const std::string &model_name, const std::string &file, NetWorkAccType type) noexcept;

  [[nodiscard]] ModelParams &get_model(const std::string &model_name) noexcept;
  [[nodiscard]] cv::dnn::Net &get_net(const std::string &model_name) noexcept;

  void clear() noexcept;

private:
  ModelLibs() = default;

private:
  std::map<std::string, ModelParams> m_models;
};

}// namespace auv::vision::network

#endif//AUV_HOST_MODEL_LIB_H
