#include "application.h"
#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>

namespace auv {

void Application::run() noexcept {
  static const char *history_file = ".auv-host.history";
  char *line;
  std::string prompt;
  read_history(history_file);
  if (m_lua["jit"].is<sol::table>()) {
    prompt = m_lua["jit"]["version"];
  } else {
    prompt = m_lua["_VERSION"];
  }
  prompt += "> ";
  while ((line = readline(prompt.c_str())) != nullptr) {
    add_history(line);
    append_history(1, history_file);
    try {
      m_lua.script(line);
    } catch (std::exception &e) {
       std::cout << "Evaluation error: " << e.what() << std::endl;
    }
  }
}


sol::state &Application::lua() {
  return m_lua;
}

}// namespace auv
