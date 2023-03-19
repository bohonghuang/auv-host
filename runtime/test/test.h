#include <functional>

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
  auv::unit_t process(int in) override {
    i = in;
    return {};
  }
  AUV_BLOCK;
  int i;
};

class TestBlock : public auv::Block<int, auv::unit_t> {
public:
  TestBlock(std::function<void(int)> pred): m_pred(pred) {}
  auv::unit_t process(int in) override {
    m_pred(in);
    return {};
  }
  AUV_BLOCK;
private:
  std::function<void(int)> m_pred;
};
