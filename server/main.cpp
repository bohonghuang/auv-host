//
// Created by Qff on 2023/2/27.
//

#include "connect_client.h"

int main() {
	auv::ConnectClient client(8888, [](std::string_view buf){
		std::cout << buf << std::endl;
	});
	client.join();
	return 0;
}
