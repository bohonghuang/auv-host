//
// Created by Qff on 2023/2/28.
//

#include "application.h"

#include <iostream>

namespace auv {

Application::Application(const std::function<void(sol::state &state)> &reg) noexcept
    : m_status(Application::Status::READY) {
  m_lua.open_libraries(sol::lib::base);
  reg(m_lua);
}

void Application::add_command(const std::string &command, const std::function<void(const std::vector<std::string> &)> &fun) noexcept {
  m_command_map[command] = fun;
  m_lua.open_libraries(sol::lib::base);
}

static std::vector<std::string> split(std::string_view str, std::string_view delim = " ") {
  std::vector<std::string> results;
  size_t begin = 0;
  size_t pos = str.find(delim);
  while (pos < str.length()) {
    size_t length = pos - begin;
    results.emplace_back(str.substr(begin, length));
    begin = pos + delim.length();
    pos = str.find(delim, begin);
  }
  if (str.length() - begin)
    results.emplace_back(str.substr(begin, str.length() - begin));
  return results;
}

[[noreturn]] void Application::run() noexcept {
  std::string msg;
  while (true) {
    std::string line;
    std::getline(std::cin, line);

    if (line[0] == '/') {
      auto params = split(line);
      if (params.empty()) {
        std::cout << "please input /help" << std::endl;
        continue;
      }

      auto it = m_command_map.find(params[0]);
      if (it == m_command_map.end()) {
        std::cout << "could not find the command" << std::endl;
        continue;
      }

      it->second(params);
      msg.clear();
    }

    if (line[line.size() - 1] == '\\') {
      msg += line;
      continue;
    }

    msg += line;

    try {
      m_lua.script(msg);
    } catch (std::exception& e) {
      // std::cout << "error: " << e.what() << std::endl;
    }

    msg.clear();
  }
}

Application::Status Application::get_status() const noexcept {
  return m_status;
}

void Application::script(const std::string &lua_code) {
  m_lua.script(lua_code);
}


}// namespace auv