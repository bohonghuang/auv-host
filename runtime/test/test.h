#include <functional>
#include <utility>

#include "block.h"

class Plus1Block : public auv::Block<int, int> {
public:
  int process(int in) override {
    return in + 1;
  }
  AUV_BLOCK;
};

class Times2Block : public auv::Block<int, int> {
public:
  int process(int in) override {
    return in * 2;
  }
  AUV_BLOCK;
};

class BufferedBlock : public auv::Block<int, auv::unit_t> {
public:
  auv::unit_t
  process(int in) override {
    i = in;
    return {};
  }
  AUV_BLOCK;
  int i;
};

class TestBlock : public auv::Block<int, auv::unit_t> {
public:
  explicit TestBlock(std::function<void(int)> pred) : m_pred(std::move(pred)) {}
  auv::unit_t
  process(int in) override {
    m_pred(in);
    return {};
  }
  AUV_BLOCK;

private:
  std::function<void(int)> m_pred;
};

class CounterBlock : public auv::Block<auv::unit_t, auv::unit_t> {
public:
  auv::unit_t
  process(auv::unit_t in) override {
    m_counter++;
    return {};
  }
  [[nodiscard]] int
  counter() const {
    return m_counter;
  }
  AUV_BLOCK;

private:
  int m_counter = 0;
};
