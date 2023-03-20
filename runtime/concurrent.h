#ifndef AUV_HOST_CONCURRENT_H
#define AUV_HOST_CONCURRENT_H

#include <any>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "block.h"

namespace auv {

struct Scheduler {
public:
  Scheduler(std::function<void(void)> task, std::chrono::milliseconds interval);
  template<class Block, class Block_ = wrap_ref_block<Block>, class = std::enable_if_t<std::is_same_v<typename Block_::In, std::any> || std::is_same_v<typename Block_::In, auv::unit_t>>>
  Scheduler(Block block, std::chrono::milliseconds interval) : Scheduler([block = Block_{std::move(block)}]() mutable { block.process(auv::unit_t{}); }, interval) {}
  ~Scheduler() {
    stop();
  }
  void start();
  void stop();
  void pause();
  void resume();

private:
  void run();
  std::function<void(void)> m_task;
  std::chrono::milliseconds m_interval{};
  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::atomic<bool> m_stopped = true, m_paused = false;
};

}// namespace auv

#endif /* AUV_HOST_CONCURRENT_H */
