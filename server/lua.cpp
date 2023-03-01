//
// Created by Qff on 2023/3/1.
//

#include "lua.h"

namespace auv::lua {


Lua::Lua()
{
	m_lua_state = luaL_newstate();
}

}
