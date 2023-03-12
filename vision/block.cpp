//
// Created by Qff on 2023/3/1.
//

#include "block.h"

namespace auv::vision {


void Block::SetFrameRate(float val) {
  if (val <= 0 || val > 60)
    return;
  s_frame_rate = val;
}


float Block::GetFrameRate() {
  return s_frame_rate;
}


AlgorithmResult Block::process(const cv::Mat &frame, TimeStep ts) noexcept {
  this->pre_process(frame);
  return this->process_imp(frame, ts);
}


void Block::pre_process(const cv::Mat &frame) noexcept {
  cv::Size size = frame.size();
//  if (size.width != m_frame_size.width || size.height != m_frame_size.height)
//    cv::resize(frame, frame, m_frame_size);
}


SingleBlock::SingleBlock(Camera &camera) noexcept
    : m_camera(camera) {
}


void SingleBlock::start() noexcept {
  if (m_is_running)
    return;

  m_is_running = true;
  std::thread thread([this]() {
    while (m_is_running) {
      float time = TimeStep::GetCurrentTime();
      static float last_time = time;
      TimeStep time_step = time - last_time;
      last_time = time;
      this->process(this->m_camera.get_frame(), time_step);
      int delay_time = static_cast<int>((1.0f / s_frame_rate - (TimeStep::GetCurrentTime() - time)) * 1000.0f);
      std::this_thread::sleep_for(std::chrono::milliseconds(delay_time));
    }
  });
  m_thread.swap(thread);
}


void SingleBlock::stop() noexcept {
  m_is_running = false;
}


const AlgorithmResult &SingleBlock::test_once() noexcept {
  // this->run();
  return m_result;
}


AlgorithmResult SingleBlock::getResult() noexcept {
  AlgorithmResult result;
  m_mutex.lock();
  result = m_result;
  m_mutex.unlock();

  return result;
}


}// namespace auv::vision