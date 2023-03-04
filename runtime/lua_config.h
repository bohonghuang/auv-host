//
// Created by Qff on 2023/3/1.
//

#ifndef LUA_CONFIG_H
#define LUA_CONFIG_H

#include <map>
#include <sol/sol.hpp>

namespace auv {

class Config {
public:
  static Config &GetInstance();

  void load(std::string_view str);
private:
  Config() = default;
  sol::state m_lua;
};

}// namespace auv

#endif//LUA_CONFIG_H
