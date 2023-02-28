//
// Created by Qff on 2023/2/27.
//

#include <iostream>
#include <fstream>

#include "connect_server.h"

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Please input the mod of the client\n"
				  << "\tinput \"auv_client 0\" to start lua-repl\n"
				  << "\tinput \"auv_client 1 {file_name ...}\" to send files to server" << std::endl;
		return 0;
	}

	if (argv[1][0] == '0') {
		std::cout << "If the end char of the line is '\\', it can prevent send the line to the server\n"
					 "Input quit to quit the program" << std::endl;

		std::string msg;
		std::string buf;
		while (true) {
			auv::ConnectServer server("192.168.31.100", 8888);
			std::cin >> buf;
			if (buf == "quit") break;
			if (buf[buf.size() - 1] == '\\') {
				msg += buf;
				continue;
			}

			msg = std::move(buf);
			auto recv = server.send_and_recv(msg);
			if(recv == "OK")
				continue;
			std::cout << recv << std::endl;
			msg.clear();
		}
	}

	if (argv[1][0] == '1') {
		size_t count = argc - 2;
		std::ifstream ifs;
		static char buf[(1024 * 1024 * 5) + 1];
		for (size_t i = 0; i < count; ++i) {
			auv::ConnectServer server("192.168.31.100", 8888);
			::memset(buf, 0, sizeof buf);

			const char* file_name = argv[2 + i];
			ifs.open(file_name);

			std::string header = "filename = ";
			header += file_name;
			header += "\r\n\r\n";

			strncpy(buf, header.c_str(), header.size());
			ifs.read(buf + header.size(), sizeof buf - header.size());
			std::cout << buf << std::endl;
			auto recv = server.send_and_recv(buf);
			ifs.close();
			if (recv != "OK") {
				std::cout << "send error, try again now!" << std::endl;
				i--;
				continue;
			}
		}
	}
	return 0;
}
