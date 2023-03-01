//
// Created by Qff on 2023/2/27.
//

#include "connect_client.h"
#include "lua.h"

int main()
{
	auv::ConnectClient client(8888, [](std::string_view buf) -> std::string
	{
		std::cout << buf << std::endl;
		return "OK";
	});
	auv::lua::Lua lua;
	auto v = lua.get_table_var<int>("abc", "a");
	client.join();
	return 0;
}
