//
// Created by Qff on 2023/3/1.
//

#ifndef BLOCK_H
#define BLOCK_H

#include <atomic>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>

#include "camera.h"
#include "common.h"

namespace auv::vision {

struct AxisResult {
  float x;
  float y;
  float z;
  float rot;
};

struct AlgorithmResult {
  std::vector<AxisResult> axis;
  cv::Mat frame;
};

class Block {
public:
  static void SetFrameRate(float val);
  static float GetFrameRate();

public:
  virtual ~Block() noexcept = default;

  AlgorithmResult process(const cv::Mat &frame) noexcept;
  // AlgorithmResult process(const cv::Mat &frame, TimeStep ts) noexcept;

protected:
  virtual void pre_process(const cv::Mat &frame) noexcept;
  virtual AlgorithmResult process_imp(const cv::Mat &frame, TimeStep ts) noexcept = 0;

  inline static float s_frame_rate = 30.0f;

protected:
  cv::Size m_frame_size = {640, 480};
};

class SingleBlock : public Block {
public:
  virtual void start(Camera &camera) noexcept;
  virtual void stop() noexcept;

  bool is_running() const noexcept;
  void join() noexcept;

  AlgorithmResult get_result() noexcept;

private:
  std::thread m_thread;
  std::mutex m_mutex;
  AlgorithmResult m_result;
  std::atomic<bool> m_is_running = false;
  std::atomic<bool> m_has_get_result = false;
};

template<typename T>
struct Range {
  T low;
  T high;
};

template<typename T>
struct ThresholdParams {
  Range<T> param1;
  Range<T> param2;
  Range<T> param3;
};

template<typename T>
class ThresholdBlock : public SingleBlock {
public:
  ThresholdParams<T> &test_get_threshold_params() {
    return m_threshold_params;
  }

  const ThresholdParams<T> &get_threshold_params() const {
    return m_threshold_params;
  }

  void set_params(T param1, T param2, T param3, T param4, T param5, T param6) {
    m_threshold_params = {
        {param1, param2},
        {param3, param4},
        {param5, param6}};
  }

  cv::Scalar get_low_param() {
    return {
        static_cast<double>(m_threshold_params.param1.low),
        static_cast<double>(m_threshold_params.param2.low),
        static_cast<double>(m_threshold_params.param3.low)};
  }

  cv::Scalar get_high_param() {
    return {
        static_cast<double>(m_threshold_params.param1.high),
        static_cast<double>(m_threshold_params.param2.high),
        static_cast<double>(m_threshold_params.param3.high)};
  }

protected:
  ThresholdParams<T> m_threshold_params;
};

}// namespace auv::vision

#endif//BLOCK_H
