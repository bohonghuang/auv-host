#ifndef AUV_HOST_RUNTIME_LUA_INTEROP_H
#define AUV_HOST_RUNTIME_LUA_INTEROP_H

#include <sol/sol.hpp>

#define AUV_BLOCK_SOL_METHODS(TYPE) "process", &TYPE::process, "as_untyped", &TYPE::as_untyped

namespace auv::lua {

void setup_env(sol::state &state);

}// namespace auv

#endif//AUV_HOST_RUNTIME_LUA_INTEROP_H
