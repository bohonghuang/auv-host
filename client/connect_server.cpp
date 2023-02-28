//
// Created by Qff on 2023/2/27.
//

#include "connect_server.h"

#include <chrono>

namespace auv
{

namespace
{
struct InitSocket
{
	InitSocket()
	{
		sockpp::initialize();
	}
};

[[maybe_unused]] static InitSocket s_init_socket;
}

ConnectServer::ConnectServer(std::string_view host, uint16_t port)
{
	using namespace std::chrono;
	m_connect = std::make_unique<sockpp::tcp_connector>(sockpp::tcp_connector::addr_t{host.data(), port},
														seconds{5});

	if (!m_connect) {
		std::cout << "Error connecting to server at "
		  		  << sockpp::inet_address(host.data(), port)
			 	  << "\n\t" << m_connect->last_error_str() << std::endl;
		return;
	}
}

ConnectServer::~ConnectServer()
{
	m_connect->shutdown();
}

std::string ConnectServer::send_and_recv(std::string_view buf)
{
	if (!m_connect->is_connected()) {
		std::cout << "connect is error" << std::endl;
		return {};
	}

	static char recv_buf[50];
	m_connect->write(buf.data(), buf.size());
	m_connect->read(recv_buf, sizeof recv_buf);
	return recv_buf;
}

}
