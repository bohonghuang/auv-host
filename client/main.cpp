//
// Created by Qff on 2023/2/27.
//

#include "connect_server.h"

int main() {
	auv::ConnectServer server("192.168.31.100", 8888);
	server.send("hello");

	std::cout << "yes" << std::endl;
	return 0;
}
