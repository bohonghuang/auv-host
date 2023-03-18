#ifndef AUV_HOST_COMMUNICATION_LUA_INTEROP_H
#define AUV_HOST_COMMUNICATION_LUA_INTEROP_H

#include <sol/sol.hpp>

namespace auv::communication::lua {

void setup_env(sol::state &state);

}

#endif//AUV_HOST_COMMUNICATION_LUA_INTEROP_H
