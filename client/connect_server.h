//
// Created by Qff on 2023/2/27.
//

#ifndef CONNECT_SERVER_H
#define CONNECT_SERVER_H

#include <memory>
#include <sockpp/tcp_connector.h>

namespace auv
{

class ConnectServer
{
public:
	ConnectServer(std::string_view, uint16_t port);

	std::string send(std::string_view buf);
private:
	std::unique_ptr<sockpp::tcp_connector> m_connect;
};

}

#endif //CONNECT_SERVER_H
