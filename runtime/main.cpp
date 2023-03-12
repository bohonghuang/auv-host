//
// Created by Qff on 2023/2/27.
//

#include "application.h"

int main() {
  struct Vars {
    int boop = 0;
  };

  Vars vars;

  auv::Application app([](sol::state& state){
    state.new_usertype<Vars>("vars", "boop", &Vars::boop);
  });

  app.run();
//  std::string l =
//      R"( print(1)
//          a = 3
//          beep = vars.new()
//          print(beep.boop)
//          beep.boop = 3
//          print(table.a)
//          print(table.b)
//          print(table.c)
//          print(table.d) )";

  std::cout << vars.boop << std::endl;
  return 0;
}
