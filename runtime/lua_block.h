#ifndef AUV_HOST_LUA_BLOCK_H
#define AUV_HOST_LUA_BLOCK_H

#include <any>
#include <sol/sol.hpp>

#include "block.h"
#include "lua_interop.h"

namespace auv {

template<class I, class O>
class LuaBlock : public Block<I, O> {
public:
  LuaBlock() : m_state(std::make_shared<sol::state>()) {}
  sol::state &
  lua() {
    return *m_state;
  }
  O process(I in) override {
    return (*m_state)["process"].call<O>(in);
  }

protected:
  std::shared_ptr<sol::state> m_state;
};


class UntypedLuaMuxBlock : public LuaBlock<unit_t, std::any>, public SharedUntypedMuxBlock {
public:
  class MuxBlock : public UntypedMuxBlock {
  public:
    std::any
    process(unit_t in) override {
      return {};
    }
  };
  UntypedLuaMuxBlock();
  std::any process(unit_t in) override;
  AUV_BLOCK
};


class UntypedLuaBlock : public LuaBlock<std::any, std::any> {
public:
  UntypedLuaBlock();
  AUV_BLOCK
};

}// namespace auv

#endif//AUV_HOST_LUA_BLOCK_H
