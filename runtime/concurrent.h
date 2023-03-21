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

class BaseScheduler {
public:
  virtual ~BaseScheduler() = default;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;
};

class Scheduler : public BaseScheduler {
public:
  Scheduler(std::function<void(void)> task, std::chrono::milliseconds interval);
  template<class Block, class Block_ = wrap_ref_block<Block>, class = std::enable_if_t<std::is_same_v<typename Block_::In, std::any> || std::is_same_v<typename Block_::In, auv::unit_t>>>
  Scheduler(Block block, std::chrono::milliseconds interval) : Scheduler([block = Block_{std::move(block)}]() mutable { block.process(auv::unit_t{}); }, interval) {}
  ~Scheduler() override;
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

class SchedulerList : public BaseScheduler {
public:
  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;
  void add(std::shared_ptr<BaseScheduler> scheduler) {
    m_list.push_back(std::move(scheduler));
  }

private:
  std::list<std::shared_ptr<BaseScheduler>> m_list{};
};

class SharedBaseScheduler : public BaseScheduler {
public:
  explicit SharedBaseScheduler(std::shared_ptr<BaseScheduler> pointer);
  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;
  std::shared_ptr<BaseScheduler> pointer();

private:
  std::shared_ptr<BaseScheduler> m_pointer;
};

class SharedScheduler : public SharedBaseScheduler {
public:
  template<class Block>
  SharedScheduler(Block block, std::chrono::milliseconds interval);
};

template<class Block>
SharedScheduler::SharedScheduler(Block block, std::chrono::milliseconds interval) : SharedBaseScheduler(std::make_shared<Scheduler>(std::move(block), interval)) {}

class SharedSchedulerList : public SharedBaseScheduler {
public:
  SharedSchedulerList();
  void add(SharedBaseScheduler& scheduler);
};

}// namespace auv

#endif /* AUV_HOST_CONCURRENT_H */
