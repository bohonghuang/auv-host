#ifndef AUV_HOST_BLOCK_H
#define AUV_HOST_BLOCK_H

#include <string>
#include <tuple>
#include <type_traits>

namespace auv {

template<class Block1, class Block2>
class ChainBlock;

template<class I, class O>
class Block {
public:
  using In = I;
  using Out = O;
  virtual O process(I in) = 0;
};

template<class Block1, class Block2>
auto operator|(Block1 block1, Block2 block2) {
  return ChainBlock<Block1, Block2>{block1, block2};
}


template<class Block1_, class Block2_>
class ChainBlock : public Block<typename std::remove_reference_t<Block1_>::In,
                                typename std::remove_reference_t<Block2_>::Out> {
  using Block1 = typename std::remove_reference_t<Block1_>;
  using Block2 = typename std::remove_reference_t<Block2_>;

  static_assert(std::is_same_v<std::remove_cv_t<std::remove_reference_t<typename Block1::Out>>, std::remove_cv_t<std::remove_reference_t<typename Block2::In>>>, "Unmatched input and output types for two blocks.");

public:
  ChainBlock(Block1 block1, Block2 block2) : m_block_1(block1), m_block_2(block2) {}
  typename Block2::Out process(typename Block1::In in) override {
    return m_block_2.process(m_block_1.process(in));
  }

private:
  Block1 m_block_1;
  Block2 m_block_2;
};

}// namespace auv

#endif//AUV_HOST_BLOCK_H
