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

Scheduler::~Scheduler() {
  stop();
}

void SchedulerList::start() {
  for (auto &scheduler: m_list) {
    scheduler->start();
  }
}

void SchedulerList::stop() {
  for (auto &scheduler: m_list) {
    scheduler->stop();
  }
}

void SchedulerList::pause() {
  for (auto &scheduler: m_list) {
    scheduler->pause();
  }
}

void SchedulerList::resume() {
  for (auto &scheduler: m_list) {
    scheduler->resume();
  }
}

SharedBaseScheduler::SharedBaseScheduler(std::shared_ptr<BaseScheduler> pointer) : m_pointer(std::move(pointer)) {}

void SharedBaseScheduler::start() {
  m_pointer->start();
}

void SharedBaseScheduler::stop() {
  m_pointer->stop();
}

void SharedBaseScheduler::pause() {
  m_pointer->pause();
}

void SharedBaseScheduler::resume() {
  m_pointer->resume();
}

std::shared_ptr<BaseScheduler>
SharedBaseScheduler::pointer() {
  return m_pointer;
}

SharedSchedulerList::SharedSchedulerList() : SharedBaseScheduler(std::make_shared<SchedulerList>()) {}

void SharedSchedulerList::add(SharedBaseScheduler &scheduler) {
  std::dynamic_pointer_cast<SchedulerList>(pointer())->add(scheduler.pointer());
}

}// namespace auv
