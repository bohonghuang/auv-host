//
// Created by Qff on 2023/2/27.
//

#include "connect_rov.h"

namespace auv
{

ConnectROV::ConnectROV(std::string_view address, uint16_t port)
	: m_connector(address.data(), port), m_rpc_client(m_connector, jsonrpccxx::version::v2)
{
}

void ConnectROV::move(float x, float y, float z, float rot)
{
	return m_rpc_client.CallNotificationNamed("move", {{"x", x}, {"y", y}, {"z", z}, {"rot", rot}});
}

void ConnectROV::move_absolute(float x, float y, float z, float rot)
{
	return m_rpc_client.CallNotificationNamed("move_absolute", {{"x", x}, {"y", y}, {"z", z}, {"rot", rot}});
}

void ConnectROV::catcher(float val)
{
	return m_rpc_client.CallNotification("catcher", {val});
}

void ConnectROV::set_direction_locked(bool val)
{
	return m_rpc_client.CallNotification("set_direction_locked", {val});
}

void ConnectROV::set_depth_locked(bool val)
{
	return m_rpc_client.CallNotification("set_depth_locked", {val});
}

std::string CppHttpLibClientConnector::Send(const std::string &request)
{
	auto res = m_http_client.Post("/jsonrpc", request, "application/json");
	if (!res || res->status != 200) {
		throw jsonrpccxx::JsonRpcException(-32003, "client connector error, received status != 200");
	}
	return res->body;
}

CppHttpLibClientConnector::CppHttpLibClientConnector(const std::string &host, int port)
	: m_http_client(host, port)
{
}

} // auv
