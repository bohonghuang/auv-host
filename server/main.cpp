//
// Created by Qff on 2023/2/27.
//

#include "connect_client.h"
#include "lua.h"

int main()
{
//	auv::ConnectClient client(8888, [](std::string_view buf) -> std::string
//	{
//		std::cout << buf << std::endl;
//		return "OK";
//	});
//	client.join();

	auv::lua::Lua lua;
	lua.set_table_var("e", {"a", "b", "c"}, 2.2, 3.3, 4.4);
	auto result = lua.get_table_var<double, double, double>("e", "a", "b", "c");
	std::string l =
		R"(for k, v in pairs(e) do print(k) print(v) end
function add(num1, num2)
	return num1 + num2
end
)";

	std::cout << lua.do_string(l) << std::endl;

	auto a = lua.call<int>("add", 1, 2);
	std::cout << "result = " << std::get<0>(a) << std::endl;
	return 0;
}
