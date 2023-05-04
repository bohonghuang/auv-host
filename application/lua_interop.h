#ifndef AUV_HOST_APPLICATION_LUA_INTEROP_H
#define AUV_HOST_APPLICATION_LUA_INTEROP_H

#include <sol/state.hpp>

namespace auv::application::lua {

void setup_env(sol::state &state);

}// namespace auv::application::lua

#endif//AUV_HOST_APPLICATION_LUA_INTEROP_H
