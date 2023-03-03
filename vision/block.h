//
// Created by Qff on 2023/3/1.
//

#ifndef BLOCK_H
#define BLOCK_H

#include <atomic>
#include <mutex>
#include <thread>

#include <opencv2/opencv.hpp>

#include "camera.h"
#include "common.h"

namespace auv::vision {

struct AlgorithmResult {
  float x;
  float y;
  float z;
  float rot;

  cv::Mat frame;
};

class Block {
public:
  static void SetFrameRate(float val);
public:
  virtual ~Block() noexcept = default;

  virtual void process(const cv::Mat &frame, TimeStep ts) noexcept = 0;
protected:
  inline static float s_frame_rate = 30.0f;
};

class SingleBlock : public Block {
public:
  SingleBlock(Camera& camera);

  void start() noexcept;
  void stop() noexcept;

  const AlgorithmResult &test_once() noexcept;

  AlgorithmResult getResult() noexcept;
private:
  Camera& m_camera;
  std::thread m_thread;
  std::mutex m_mutex;
  AlgorithmResult m_result;
  std::atomic<bool> m_is_running = false;
};

}// namespace auv::algorithm

#endif//BLOCK_H
