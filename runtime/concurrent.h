#ifndef AUV_HOST_CONCURRENT_H
#define AUV_HOST_CONCURRENT_H

#include <any>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

#include "block.h"

namespace auv {

class Schedulable {
public:
  virtual ~Schedulable() = default;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;
};

class Scheduler : public Schedulable {
public:
  Scheduler(std::function<void(void)> task, std::chrono::milliseconds interval);
  template<class Block, class Block_ = wrap_ref_block<Block>, class = std::enable_if_t<std::is_same_v<typename Block_::In, std::any> || std::is_same_v<typename Block_::In, auv::unit_t>>>
  Scheduler(Block block, std::chrono::milliseconds interval) : Scheduler([block = Block_{std::move(block)}]() mutable { block.process(auv::unit_t{}); }, interval) {}
  ~Scheduler() {
    stop();
  }
  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

private:
  void run();
  std::function<void(void)> m_task;
  std::chrono::milliseconds m_interval{};
  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::atomic<bool> m_stopped = true, m_paused = false;
};

class SchedulerList : public Schedulable {
public:
  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

private:
  std::list<std::shared_ptr<Schedulable>> m_list {};
};

}// namespace auv

#endif /* AUV_HOST_CONCURRENT_H */
