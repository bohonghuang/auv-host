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
  LuaBlock(): m_state(std::make_shared<sol::state>()) {}
  sol::state &lua() {
    return *m_state;
  }
  O process(I in) override {
    return (*m_state)["process"].call<O>(in);
  }
protected:
  std::shared_ptr<sol::state> m_state;
};

class UntypedLuaMuxBlock : public LuaBlock<unit_t, std::any> {
public:
  UntypedLuaMuxBlock();
  std::any process(unit_t in) override;
  UntypedMuxBlock::InputBlock input_block(UntypedMuxBlock::Key key) {
    return {m_block, std::move(key)};
  }
  AUV_BLOCK
private:
  std::shared_ptr<UntypedMuxBlock> m_block;
};

class UntypedLuaBlock : public LuaBlock<std::any, std::any> {
public:
  UntypedLuaBlock();
  AUV_BLOCK
};

}

#endif//AUV_HOST_LUA_BLOCK_H
