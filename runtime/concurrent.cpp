#include "concurrent.h"

#include <utility>

namespace auv {

Scheduler::Scheduler(std::function<void(void)> task, std::chrono::milliseconds interval) : m_task(std::move(task)), m_interval(interval) {}

void Scheduler::start() {
  if (!m_stopped) return;
  m_stopped = m_paused = false;
  m_thread = std::thread([this] { run(); });
}

void Scheduler::stop() {
  m_stopped = true;
  if (m_thread.joinable())
    m_thread.join();
}

void Scheduler::pause() {
  m_paused = true;
}

void Scheduler::resume() {
  m_paused = false;
}

void Scheduler::run() {
  while (!m_stopped) {
    m_task();
    {
      std::unique_lock<std::mutex> lock{m_mutex};
      do {
        m_cv.wait_for(lock, m_interval);
        if (m_stopped) break;
      } while (m_paused);
    }
  }
}

}// namespace auv
