#include "rov_controller.h"

namespace auv {

RovController::RovController(const std::string &address, int port) noexcept
    : m_connector(address, port), m_rpc_client(m_connector, jsonrpccxx::version::v2) {
  if (m_connector.is_valid()) {
    std::cout << "Connect Rov success. address : " << address << "   port : " << port << std::endl;
  } else {
    std::cout << "Connect Rov error!!" << std::endl;
  }
}


void RovController::move(float x, float y, float z, float rot) noexcept {
  return m_rpc_client.CallNotificationNamed("move", {{"x", x}, {"y", y}, {"z", z}, {"rot", rot}});
}

void RovController::move_absolute(float x, float y, float z, float rot) noexcept {
  return m_rpc_client.CallNotificationNamed("move_absolute", {{"x", x}, {"y", y}, {"z", z}, {"rot", rot}});
}

void RovController::catcher(float val) noexcept {
  return m_rpc_client.CallNotification("catcher", {val});
}

void RovController::set_direction_locked(bool val) noexcept {
  return m_rpc_client.CallNotification("set_direction_locked", {val});
}

void RovController::set_depth_locked(bool val) noexcept {
  return m_rpc_client.CallNotification("set_depth_locked", {val});
}

std::string CppHttpLibClientConnector::Send(const std::string &request) noexcept {
  auto res = m_http_client.Post("/jsonrpc", request, "application/json");
  if (!res || res->status != 200) {
    // throw jsonrpccxx::JsonRpcException(-32003, "client connector error, received status != 200");
    return "";
  }
  return res->body;
}


CppHttpLibClientConnector::CppHttpLibClientConnector(const std::string &host, int port) noexcept
    : m_http_client(host, port) {
}

bool CppHttpLibClientConnector::is_valid() const noexcept {
  return m_http_client.is_valid();
}


}// namespace auv
