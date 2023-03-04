//
// Created by Qff on 2023/2/27.
//

#include <iostream>
#include <sol/sol.hpp>

int main() {
  //	auv::ConnectClient client(8888, [](std::string_view buf) -> std::string
  //	{
  //		std::cout << buf << std::endl;
  //		return "OK";
  //	});
  //	client.join();

  sol::state lua;
  lua.open_libraries(sol::lib::base);

  struct vars {
    int boop = 0;
  };
  lua.new_usertype<vars>("vars", "loop", &vars::boop);

  lua["table"] = lua.create_table_with("value", 33);
  lua["table"] = lua.create_table_with("a", 2, "b", 3, "c", 6);
  lua["table"]["d"] = ("666");
  std::string l =
      R"( print(1)
          a = 3
          beep = vars.new()
          print(beep)
          print(table.a)
          print(table.b)
          print(table.c)
          print(table.d) )";
  lua.script(l);

  return 0;
}
