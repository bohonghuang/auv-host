#ifndef AUV_HOST_APPLICATION_LUA_INTEROP_H
#define AUV_HOST_APPLICATION_LUA_INTEROP_H

#include <sol/state.hpp>

#include "runtime/lua_interop.h"
#include "vision/lua_interop.h"
#include "communication/lua_interop.h"

namespace auv::application::lua {

void setup_env(sol::state &state);

inline static void (*setup_env_all)(sol::state &) = auv::lua::setup_env_all = &setup_env;

}// namespace auv::application::lua

#endif//AUV_HOST_APPLICATION_LUA_INTEROP_H
