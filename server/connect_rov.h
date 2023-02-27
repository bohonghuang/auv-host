//
// Created by Qff on 2023/2/27.
//

#ifndef AUV_HOST_V2_CONNECT_ROV_H
#define AUV_HOST_V2_CONNECT_ROV_H

#include "jsonrpccxx/client.hpp"
#include "cpp-httplib/httplib.h"

namespace auv
{

class CppHttpLibClientConnector: public jsonrpccxx::IClientConnector
{
public:
	CppHttpLibClientConnector(const std::string &host, int port);
	std::string Send(const std::string &request) override;
private:
	httplib::Client m_http_client;
};

class ConnectROV
{
public:
	ConnectROV(std::string_view address = "192.168.137.219", uint16_t port = 8888);

	void catcher(float val);
	void move(float x, float y, float z, float rot);
	void move_absolute(float x, float y, float z, float rot);
	void set_direction_locked(bool val);
	void set_depth_locked(bool val);
private:
	CppHttpLibClientConnector m_connector;
	jsonrpccxx::JsonRpcClient m_rpc_client;
};

}

#endif //AUV_HOST_V2_CONNECT_ROV_H
