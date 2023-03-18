#ifndef AUV_HOST_VISION_LUA_INTEROP_H
#define AUV_HOST_VISION_LUA_INTEROP_H

#include<sol/sol.hpp>

namespace auv::vision::lua {

void setup_env(sol::state &state);

}

#endif//AUV_HOST_VISION_LUA_INTEROP_H
