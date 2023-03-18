#ifndef APPLICATION_H
#define APPLICATION_H

#include <sol/sol.hpp>

namespace auv {

class Application {
public:
  void run() noexcept;
private:
  sol::state m_lua {};

public:
  [[nodiscard]] sol::state &lua();
};

}// namespace auv


#endif//APPLICATION_H
