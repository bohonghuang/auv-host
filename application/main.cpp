#include "application.h"
#include "lua_interop.h"

int main() {
  auv::Application app;
  auv::application::lua::setup_env(app.lua());
  app.run();
}
