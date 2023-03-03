//
// Created by Qff on 2023/2/28.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <functional>
#include <map>
#include <thread>


namespace auv {

class Application {
public:
  static Application &GetInstance() noexcept;

  void start() noexcept;
  void reload() noexcept;
  void stop() noexcept;


private:
  Application() = default;
  void run() noexcept;

private:
  std::map<std::string, std::function<void()>> m_algorithm_funcs;
};

}// namespace auv


#endif//APPLICATION_H
