#include "application.h"
#include "lua_interop.h"
#include "vision/camera_mgr.h"
#include "vision/nn/model_lib.h"

int main() {
  auv::Application app;
  auv::application::lua::setup_env(app.lua());
  app.run();

  auv::vision::network::ModelLibs::GetInstance().clear();
}
