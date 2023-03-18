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
