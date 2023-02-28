#ifndef AUV_HOST_V2_CONNECT_CLIENT_H
#define AUV_HOST_V2_CONNECT_CLIENT_H

#include <thread>
#include <memory>
#include <functional>
#include <sockpp/tcp_acceptor.h>

namespace auv
{

class ConnectClient {
public:
	ConnectClient(uint16_t port, std::function<std::string(std::string_view)> callback) noexcept;

	void join();
private:
	[[noreturn]] void run();
private:
	std::function<std::string(std::string_view)> m_cb;
	std::unique_ptr<sockpp::tcp_acceptor> m_accept;
	std::thread m_thread;
};

}

#endif //AUV_HOST_V2_CONNECT_CLIENT_H
