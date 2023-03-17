//
// Created by Qff on 2023/3/14.
//

#ifndef CONNECT_ROV_H
#define CONNECT_ROV_H

#include <cpp-httplib/httplib.h>
#include <jsonrpccxx/client.hpp>

namespace auv {

class CppHttpLibClientConnector : public jsonrpccxx::IClientConnector {
public:
  CppHttpLibClientConnector(const std::string &host, int port) noexcept;
  std::string Send(const std::string &request) noexcept override;

private:
  httplib::Client m_http_client;
};

class ConnectROV {
public:
  explicit ConnectROV(const std::string &address = "192.168.137.219", int port = 8888) noexcept;

  void catcher(float val) noexcept;
  void move(float x, float y, float z, float rot) noexcept;
  void move_absolute(float x, float y, float z, float rot) noexcept;
  void set_direction_locked(bool val) noexcept;
  void set_depth_locked(bool val) noexcept;

private:
  CppHttpLibClientConnector m_connector;
  jsonrpccxx::JsonRpcClient m_rpc_client;
};


}// namespace auv

#endif//CONNECT_ROV_H
