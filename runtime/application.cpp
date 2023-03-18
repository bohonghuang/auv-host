//
// Created by Qff on 2023/2/28.
//

#include "application.h"
#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>

namespace auv {

Application::Application(const std::function<void(sol::state &state)> &reg) noexcept
    : m_status(Application::Status::READY) {
  m_lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::jit);
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

void Application::run() noexcept {
  char *line;
  std::string prompt;
  if (m_lua["jit"].is<sol::table>()) {
    prompt = m_lua["jit"]["version"];
  } else {
    prompt = m_lua["_VERSION"];
  }
  prompt += "> ";
  while ((line = readline(prompt.c_str())) != nullptr) {
    add_history(line);
    try {
      m_lua.script(line);
    } catch (std::exception &e) {
       std::cout << "Evaluation error: " << e.what() << std::endl;
    }
  }
}

Application::Status Application::get_status() const noexcept {
  return m_status;
}

void Application::script(const std::string &lua_code) {
  m_lua.script(lua_code);
}


}// namespace auv
