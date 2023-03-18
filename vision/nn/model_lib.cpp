//
// Created by qff233 on 23-3-18.
//

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
  cv::dnn::Net net;
  net = cv::dnn::readNet(file);
  switch (type) {
    case NetWorkAccType::CPU:
      net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
      net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
      break;
    case NetWorkAccType::GPU:
      net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
      net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
      break;
  }
  m_models[model_name] = {type, net};
}

ModelParams &ModelLibs::get_model(const std::string &model_name) noexcept {
  auto it = m_models.find(model_name);
  if (it == m_models.end())
    ASSERT(false, "could not find the model!")
  return it->second;
}

cv::dnn::Net &ModelLibs::get_net(const std::string &model_name) noexcept {
  auto &model = this->get_model(model_name);
  return model.net;
}

}// namespace auv::vision::network
