#include "model_lib.h"
#include "common.h"

namespace auv::vision::network {


ModelLibs &ModelLibs::GetInstance() noexcept {
  static ModelLibs model_libs;
  return model_libs;
}


void ModelLibs::add_model(const std::string &model_name, const std::string &file, NetWorkAccType type) noexcept {
  auto it = m_models.find(model_name);
  if (it != m_models.end())
    ASSERT(false, "This model has been in the libs")
  m_models[model_name] = {type, cv::dnn::Net(), file, false};
}


ModelParams &ModelLibs::get_model(const std::string &model_name) noexcept {
  auto it = m_models.find(model_name);
  if (it == m_models.end())
    ASSERT(false, "could not find the model!")

  if (!it->second.is_loaded) {
    auto &param = it->second;
    param.is_loaded = true;

    cv::dnn::Net net = cv::dnn::readNet(param.file_name);
    if (net.empty())
      ASSERT(false, "could not load the onnx")

    switch (param.acc_type) {
      case NetWorkAccType::CPU:
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        break;
      case NetWorkAccType::GPU:
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        break;
    }
    param.net = net;
  }

  return it->second;
}


cv::dnn::Net& ModelLibs::get_net(const std::string &model_name) noexcept {
  return this->get_model(model_name).net;
}


void ModelLibs::clear() noexcept {
  m_models.clear();
}

}// namespace auv::vision::network
