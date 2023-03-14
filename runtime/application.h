//
// Created by Qff on 2023/2/28.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <functional>
#include <map>
#include <thread>

#include <sol/sol.hpp>

namespace auv {

class Application {
public:
  enum class Status {
    READY,
    RUN
  };

  Application(const std::function<void(sol::state&)>& reg) noexcept;

  void add_command(const std::string& command, const std::function<void(const std::vector<std::string>&)>& fun) noexcept;
  void script(const std::string& lua_code);

  [[noreturn]]
  void run() noexcept;

  Status get_status() const noexcept;
private:
  Status m_status;
  sol::state m_lua;
  std::map<std::string, std::function<void(const std::vector<std::string>&)>> m_command_map;
};

}// namespace auv


#endif//APPLICATION_H
