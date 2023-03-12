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

  AlgorithmResult process(const cv::Mat &frame, TimeStep ts = 0.0f) noexcept;

protected:
  virtual void pre_process(const cv::Mat &frame) noexcept;
  virtual AlgorithmResult process_imp(const cv::Mat &frame, TimeStep ts) noexcept = 0;

  inline static float s_frame_rate = 30.0f;

protected:
  cv::Size m_frame_size = {640, 480};
};

class SingleBlock : public Block {
public:
  SingleBlock(Camera &camera) noexcept;

  virtual void start() noexcept;
  virtual void stop() noexcept;

  const AlgorithmResult &test_once() noexcept;

  AlgorithmResult getResult() noexcept;

private:
  Camera &m_camera;
  std::thread m_thread;
  std::mutex m_mutex;
  AlgorithmResult m_result;
  std::atomic<bool> m_is_running = false;
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
class ThresholdBlock : public Block {
public:
  ThresholdParams<T> &test_get_threshold_params() {
    return m_threshold_params;
  }

  const ThresholdParams<T> &get_threshold_params() const {
    return m_threshold_params;
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
