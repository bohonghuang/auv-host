//
// Created by Qff on 2023/2/28.
//

#include "application.h"

#include <iostream>
#include <opencv2/opencv.hpp>

namespace auv {

Application::Application(const std::function<void(sol::state &state)> &reg) noexcept
    : m_status(Application::Status::READY) {
  m_lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::package);
//  m_lua.new_usertype<auv::vision::Block>(
//      "Block",
//      "process", &auv::vision::Block::process);
//
//  m_lua.new_usertype<auv::vision::SingleBlock>(
//      "SingleBlock",
//      "start", &auv::vision::SingleBlock::start,
//      "stop", &auv::vision::SingleBlock::stop,
//      "is_running", &auv::vision::SingleBlock::is_running,
//      "get_result", &auv::vision::SingleBlock::get_result);
//
//  m_lua.new_usertype<auv::vision::ThresholdBlock<int>>(
//      "ThresholdBlock",
//      sol::base_classes, sol::bases<auv::vision::SingleBlock>(),
//      "set_params", &auv::vision::ThresholdBlock<int>::set_params);
//
//  m_lua.new_usertype<auv::vision::CameraBlock>(
//      "CameraBlock",
//      sol::constructors<auv::vision::CameraBlock(int, double, double, double, double, double, double, double, double, double)>(),
//      "get_frame", &auv::vision::CameraBlock::get_frame);
//
//  m_lua.new_usertype<auv::vision::AxisResult>(
//      "AxisResult",
//      "x", &auv::vision::AxisResult::x,
//      "y", &auv::vision::AxisResult::y,
//      "z", &auv::vision::AxisResult::z,
//      "rot", &auv::vision::AxisResult::rot);

  m_lua.new_usertype<auv::ConnectROV>(
      "ROV",
      sol::constructors<auv::ConnectROV(const std::string &, int)>(),
      "catcher", &auv::ConnectROV::catcher,
      "move", &auv::ConnectROV::move,
      "move_absolute", &auv::ConnectROV::move_absolute,
      "set_direction_locked", &auv::ConnectROV::set_direction_locked,
      "set_depth_locked", &auv::ConnectROV::set_depth_locked);

//  m_lua.new_usertype<auv::vision::AlgorithmResult>(
//      "AlgorithmResult",
//      "axis", &auv::vision::AlgorithmResult::axis,
//      "frame", &auv::vision::AlgorithmResult::frame);

  m_lua.set_function("imshow", [](const cv::Mat &frame) {
    cv::imshow("pre", frame);
    cv::waitKey(1);
  });
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
    if (line.empty())
      continue;
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
      continue;
    }

    if (line[line.size() - 1] == '\\') {
      msg += line;
      continue;
    }

    msg += line;

    try {
      m_lua.script(msg);
    } catch (std::exception &e) {
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
