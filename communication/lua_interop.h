#ifndef AUV_HOST_COMMUNICATION_LUA_INTEROP_H
#define AUV_HOST_COMMUNICATION_LUA_INTEROP_H

#include <sol/sol.hpp>

#include "runtime/lua_interop.h"

namespace auv::communication::lua {

void setup_env(sol::state &state);

inline static void (*setup_env_all)(sol::state &) = auv::lua::setup_env_all = &setup_env;

}

#endif//AUV_HOST_COMMUNICATION_LUA_INTEROP_H
