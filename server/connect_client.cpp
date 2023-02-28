//
// Created by Qff on 2023/2/27.
//

#include "connect_client.h"

#include <utility>

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

ConnectClient::ConnectClient(uint16_t port, std::function<std::string(std::string_view)> callback) noexcept
	: m_cb(std::move(callback))
{
	m_accept = std::make_unique<sockpp::tcp_acceptor>(port);
	if (!m_accept) {
		std::cout << "Error creating the acceptor: " << m_accept->last_error_str() << std::endl;
		return;
	}

	m_thread = std::thread([this]()
						   { this->run(); });
}

[[noreturn]]
void ConnectClient::run()
{
	sockpp::inet_address peer;
	static char buf[1024 * 1024 * 5];
	while (true) {
		sockpp::tcp_socket sock = m_accept->accept(&peer);
		if (!sock) {
			std::cout << "Error accepting incoming connection: "
					  << m_accept->last_error_str() << std::endl;
			continue;
		}

		size_t n = sock.read(buf, sizeof(buf));
		auto msg = m_cb(buf);
		sock.write(msg);
		memset(buf, 0, sizeof(buf));
	}
}

void ConnectClient::join()
{
	m_thread.join();
}

}