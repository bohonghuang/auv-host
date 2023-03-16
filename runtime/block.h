#ifndef AUV_HOST_BLOCK_H
#define AUV_HOST_BLOCK_H

#include <any>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>

namespace auv {

namespace self_type {
template<typename T>
struct Reader {
  friend auto adl_GetSelfType(Reader<T>);
};

template<typename T, typename U>
struct Writer {
  friend auto adl_GetSelfType(Reader<T>) { return U{}; }
};

inline void adl_GetSelfType() {}

template<typename T>
using Read = std::remove_pointer_t<decltype(adl_GetSelfType(Reader<T>{}))>;
}// namespace self_type


#define DEFINE_SELF                                                                                                 \
  struct _self_type_tag {};                                                                                         \
  constexpr auto _self_type_helper()->decltype(auv::self_type::Writer<_self_type_tag, decltype(this)>{}, void()) {} \
  using Self = auv::self_type::Read<_self_type_tag>;

#define AUV_BASIC_BLOCK                        \
  DEFINE_SELF                                  \
  auto as_into_untyped() {                     \
    return auv::as_into_untyped_block(*this);  \
  }                                            \
  auto as_from_untyped() {                     \
    return auv::as_from_untyped_block(*this);  \
  }                                            \
  auto as_untyped() {                          \
    return auv::as_untyped_block(*this);       \
  }                                            \
  template<class I_, class O_>                 \
  auto as_typed() {                            \
    return auv::as_typed_block<I_, O_>(*this); \
  }

#define AUV_BLOCK                        \
  AUV_BASIC_BLOCK                        \
  template<class... Arg>                 \
  static Self make_typed(Arg... arg) {   \
    return Self(arg...);                 \
  }                                      \
  template<class... Arg>                 \
  static auto make_untyped(Arg... arg) { \
    return Self(arg...).as_untyped();    \
  }

template<class Block1, class Block2>
class ChainBlock;

template<class I>
class IntoAnyBlock;

template<class O>
class FromAnyBlock;

template<class T>
class IdentityBlock;

class AnyBlock;

template<class I, class O>
class TypedAnyBlock;

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

template<class Block>
auto as_from_untyped_block(Block block) -> ChainBlock<FromAnyBlock<typename Block::In>, Block> {
  return {{}, block};
}

template<class Block>
auto as_into_untyped_block(Block block) -> ChainBlock<Block, IntoAnyBlock<typename Block::Out>> {
  return {block, {}};
}

template<class Block, class Result = AnyBlock>
auto as_untyped_block(Block block) -> Result {
  return as_into_untyped_block(as_from_untyped_block(block));
}

template<class I, class O, class Arg = AnyBlock>
auto as_typed_block(Arg block) -> TypedAnyBlock<I, O> {
  return block;
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
  AUV_BASIC_BLOCK;

private:
  Block1 m_block_1;
  Block2 m_block_2;
};

template<class T>
class IdentityBlock : public Block<T, T> {
public:
  T process(T in) override {
    return in;
  }
  AUV_BASIC_BLOCK;
};

template<class I>
class IntoAnyBlock : public Block<I, std::any> {
public:
  std::any process(I in) override {
    return {in};
  }
  AUV_BASIC_BLOCK;
};

template<class O>
class FromAnyBlock : public Block<std::any, O> {
public:
  O process(std::any in) override {
    return std::any_cast<O>(in);
  }
  AUV_BASIC_BLOCK;
};

class AnyBlock : public Block<std::any, std::any> {
public:
  template<class B>
  AnyBlock(B block) : m_block(std::make_shared<B>(block)) {}
  std::any process(std::any in) override {
    return m_block->process(in);
  }
  AUV_BASIC_BLOCK;
  AnyBlock connect(AnyBlock block) {
    return (*this | block).as_untyped();
  }

private:
  std::shared_ptr<Block<std::any, std::any>> m_block;
};

template<class I, class O>
class TypedAnyBlock : public Block<I, O> {
public:
  TypedAnyBlock(AnyBlock block) : m_block(block) {}
  O process(I in) override {
    return std::any_cast<O>(m_block.process(std::any{in}));
  }
  AUV_BASIC_BLOCK;

private:
  AnyBlock m_block;
};

}// namespace auv

#endif//AUV_HOST_BLOCK_H
