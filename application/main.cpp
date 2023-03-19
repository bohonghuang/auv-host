#include "application.h"
#include "lua_interop.h"
#include "vision/camera_mgr.h"
#include "vision/nn/model_lib.h"

int main() {
  auv::Application app;

  auv::vision::network::ModelLibs::GetInstance()
      .add_model("marine", "./model_data/marine.onnx",
                 auv::vision::network::NetWorkAccType::GPU);

  auv::vision::CameraManager::GetInstance()
      .add_capture({{0, {640, 480}},
                    {2, {640, 480}}});

  auv::application::lua::setup_env(app.lua());
  app.run();

  auv::vision::network::ModelLibs::GetInstance().clear();
}
