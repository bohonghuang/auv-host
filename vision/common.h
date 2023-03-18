#ifndef TIME_STEP_H
#define TIME_STEP_H

#ifdef NDEBUG
#define ASSERT(expression, information)
#else
#define ASSERT(expression, information)                     \
  {                                                         \
    assert(expression);                                     \
    if (!expression)                                        \
      std::cout << "assert : " << information << std::endl; \
  }
#endif

#include <opencv2/core/utility.hpp>

namespace auv::vision {

using vPointfs = std::vector<cv::Point2f>;
using vPoints = std::vector<cv::Point>;

class TimeStep {
public:
  static float GetCurrentTime() {
    return static_cast<float>((double) cv::getTickCount() / cv::getTickFrequency());
  }

public:
  TimeStep(float val) : m_time_step(val) {}
  operator float() const {
    return m_time_step;
  }

  float get_seconds() const { return m_time_step; }
  float get_milliseconds() const { return m_time_step * 1000.0f; }

private:
  float m_time_step;
};

}// namespace auv::vision

#endif//TIME_STEP_H
