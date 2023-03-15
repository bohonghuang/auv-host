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


//AlgorithmResult Block::process(const cv::Mat &frame, TimeStep ts) noexcept {
//  this->pre_process(frame);
//  return this->process_imp(frame, ts);
//}

AlgorithmResult Block::process(const cv::Mat &frame) noexcept {
  this->pre_process(frame);
  return this->process_imp(frame, 0.0);
}

void Block::pre_process(const cv::Mat &frame) noexcept {
  cv::Size size = frame.size();
  //  if (size.width != m_frame_size.width || size.height != m_frame_size.height)
  //    cv::resize(frame, frame, m_frame_size);
}

void SingleBlock::start(Camera &camera) noexcept {
  if (m_is_running)
    return;

  m_is_running = true;
  m_thread = std::thread([this, &camera]() {
    while (m_is_running) {
      float time = TimeStep::GetCurrentTime();
      static float last_time = time;
      TimeStep time_step = time - last_time;
      last_time = time;
      // this->process(this->m_camera.get_frame(), time_step);
      m_mutex.lock();
      m_result = this->process(camera.get_frame());
      m_has_get_result = true;
      m_mutex.unlock();
      int delay_time = static_cast<int>((1.0f / s_frame_rate - (TimeStep::GetCurrentTime() - time)) * 1000.0f);
      // std::cout << delay_time << std::endl;
      if (delay_time > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_time));
    }
  });
}


void SingleBlock::stop() noexcept {
  m_is_running = false;
  m_thread.join();
}


AlgorithmResult SingleBlock::get_result() noexcept {
  AlgorithmResult result;
  do {
    m_mutex.lock();
    result = m_result;
    m_mutex.unlock();
  } while (!m_has_get_result);

  return result;
}


void SingleBlock::join() noexcept {
  m_thread.join();
}


bool SingleBlock::is_running() const noexcept {
  return m_is_running;
}


}// namespace auv::vision