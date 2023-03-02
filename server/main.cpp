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
//	lua.set_table_var("e", {"a", "b", "c"}, 2.2, 3.3, 4.4);
//	auto result = lua.get_table_var<double, double, double>("e", "a", "b", "c");

//	auv::lua::LuaModule mod{
//		"test",
//		{
//			{"print", [](lua_State *L)
//			{
//				std::cout << "hello" << std::endl;
//				return 0;
//			}},
//			{"add", [](lua_State *L)
//			{
//				double a = lua_tonumber(L, -1);
//				double b = lua_tonumber(L, -2);
//				lua_pop(L, 2);
//				lua_pushnumber(L, a+b);
//				return 1;
//			}}
//		}
//	};
//lua.add_module(mod);
	lua.add_function("test", "test", [](lua_State *L) -> int
	{
		std::cout << "hello!" << std::endl;
		return 0;
	});
	lua.add_function("test", "add", [](lua_State *L) -> int
	{
		double a = lua_tonumber(L, -1);
		double b = lua_tonumber(L, -2);
		lua_pop(L, 2);
		lua_pushnumber(L, a + b);
		return 1;
	});

	lua.add_function("testa", [](lua_State *L) -> int
	{
		std::cout << "hello!" << std::endl;
		return 0;
	});
	lua.add_function("add1", [](lua_State *L) -> int
	{
		double a = lua_tonumber(L, -1);
		double b = lua_tonumber(L, -2);
		lua_pop(L, 2);
		lua_pushnumber(L, a + b);
		return 1;
	});
	std::string l =
		R"(
function add(num1, num2)
	return num1 + num2
end
test.test()
print(test.add(1,2))
testa()
print(add1(1, 3))
)";

	std::cout << lua.do_string(l) << std::endl;

	// auto a = lua.call<int>("add", 1, 2);
	// std::cout << "result = " << std::get<0>(a) << std::endl;
	return 0;
}
