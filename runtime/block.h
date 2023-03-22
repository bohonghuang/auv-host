#ifndef AUV_HOST_BLOCK_H
#define AUV_HOST_BLOCK_H

#include <any>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace auv {

struct unit_t {};

namespace self_type {
template<typename T>
struct Reader {
  friend auto adl_GetSelfType(Reader<T>);
};

template<typename T, typename U>
struct Writer {
  friend auto
  adl_GetSelfType(Reader<T>) { return U{}; }
};

inline void
adl_GetSelfType() {}

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

#define AUV_BLOCK                                        \
  AUV_BASIC_BLOCK                                        \
  template<class... Arg>                                 \
  static Self make_typed(Arg... arg) {                   \
    return Self(arg...);                                 \
  }                                                      \
  template<class... Arg>                                 \
  static auv::AnyBlock make_untyped(Arg... arg) {        \
    return Self(arg...).as_untyped();                    \
  }                                                      \
  template<class... Arg>                                 \
  static std::shared_ptr<Self> make_shared(Arg... arg) { \
    return std::make_shared<Self>(arg...);               \
  }                                                      \
  template<class... Arg>                                 \
  static std::unique_ptr<Self> make_unique(Arg... arg) { \
    return std::make_unique<Self>(arg...);               \
  }


#define AUV_MUX_BLOCK                                    \
  AUV_BASIC_BLOCK                                        \
  template<class... Arg>                                 \
  static std::shared_ptr<Self> make_shared(Arg... arg) { \
    auto ptr = std::make_shared<Self>(arg...);           \
    ptr->m_ref_self = ptr;                               \
    return ptr;                                          \
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

template<class B>
class SharedBlock;

template<class I, class O>
class Block {
public:
  using In = I;
  using Out = O;
  virtual O process(I in) = 0;
};

template<class T>
struct wrap_ref_block_impl {
  using type = T;
};

template<class T>
struct wrap_ref_block_impl<std::shared_ptr<T>> {
  using type = SharedBlock<T>;
};

template<class T>
using wrap_ref_block = typename wrap_ref_block_impl<T>::type;

template<class Block1, class Block2>
auto operator|(Block1 block1, Block2 block2) {
  using Block1_ = wrap_ref_block<Block1>;
  using Block2_ = wrap_ref_block<Block2>;
  return ChainBlock<Block1_, Block2_>{Block1_{block1}, Block2_{block2}};
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
  ChainBlock(Block1 block1, Block2 block2) : m_block_1(std::move(block1)), m_block_2(std::move(block2)) {}

  typename Block2::Out
  process(typename Block1::In in) override {
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
  std::any
  process(I in) override {
    return in;
  }
  AUV_BASIC_BLOCK;
};

template<class O>
class FromAnyBlock : public Block<std::any, O> {
public:
  O process(std::any in) override {
    if constexpr (std::is_same_v<O, std::any>) {
      return in;
    } else {
      return std::any_cast<O>(in);
    }
  }
  AUV_BASIC_BLOCK;
};

class AnyBlock : public Block<std::any, std::any> {
public:
  template<class B>
  AnyBlock(B block) : m_block(std::make_shared<B>(block)) {}
  std::any
  process(std::any in) override {
    return m_block->process(in);
  }
  AUV_BASIC_BLOCK;
  AnyBlock
  connect(AnyBlock block) {
    return (*this | std::move(block)).as_untyped();
  }

private:
  std::shared_ptr<Block<std::any, std::any>> m_block;
};

template<class I, class O>
class TypedAnyBlock : public Block<I, O> {
public:
  TypedAnyBlock(AnyBlock block) : m_block(std::move(block)) {}
  O process(I in) override {
    return std::any_cast<O>(m_block.process(std::any{in}));
  }
  AUV_BASIC_BLOCK;

private:
  AnyBlock m_block;
};

template<class T>
struct member_object_pointer_type_impl {};

template<class T, class S>
struct member_object_pointer_type_impl<T S::*> {
  using target = T;
  using parent = S;
};

template<class T>
using member_object_pointer_target_t = typename member_object_pointer_type_impl<T>::target;

template<class T>
using member_object_pointer_parent_t = typename member_object_pointer_type_impl<T>::parent;

template<class B>
class SharedBlock : public Block<typename B::In, typename B::Out> {
public:
  explicit SharedBlock(std::shared_ptr<B> ptr) : m_ptr(ptr) {}
  explicit SharedBlock(B &&b) : m_ptr(std::make_shared<B>(std::move(b))) {}
  typename B::Out
  process(typename B::In in) override {
    return m_ptr->process(in);
  }

protected:
  std::shared_ptr<B> m_ptr;
};


class UntypedMuxBlock : public Block<unit_t, std::any> {
public:
  using Key = std::string;
  class InputBlock : public Block<std::any, unit_t> {
  public:
    InputBlock(std::weak_ptr<UntypedMuxBlock> parent, Key key) : m_ref_parent(std::move(parent)), m_key(std::move(key)) {}

    unit_t
    process(std::any in) override {
      if (auto parent = m_ref_parent.lock()) {
        auto lock_guard = parent->buffer_lock_guard();
        parent->m_buffer[m_key] = in;
      }
      return {};
    }
    AUV_BLOCK;

  private:
    std::weak_ptr<UntypedMuxBlock> m_ref_parent;
    Key m_key;
  };

  std::lock_guard<std::mutex>
  buffer_lock_guard() {
    return std::lock_guard<std::mutex>{m_buffer_mutex};
  }

  const std::unordered_map<Key, std::any> &
  buffer() const {
    return m_buffer;
  }

private:
  std::mutex m_buffer_mutex{};
  std::unordered_map<Key, std::any> m_buffer{};
};

class SharedUntypedMuxBlock : public SharedBlock<UntypedMuxBlock> {
public:
  using SharedBlock<UntypedMuxBlock>::SharedBlock;

  UntypedMuxBlock::InputBlock
  input_block(UntypedMuxBlock::Key key) {
    return {m_ptr, key};
  }

protected:
  std::lock_guard<std::mutex>
  buffer_lock_guard() {
    return m_ptr->buffer_lock_guard();
  }
  const std::unordered_map<UntypedMuxBlock::Key, std::any> &m_buffer = m_ptr->buffer();
};

template<class Block1, class Block2, class In = std::common_type_t<typename Block1::In, typename Block2::In>>
class TeeBlock : public Block<In, unit_t> {
public:
  TeeBlock(Block1 block1, Block2 block2) : m_block_1(block1), m_block_2(block2) {}

  unit_t
  process(In in) override {
    m_block_1.process(in);
    m_block_2.process(in);
    return {};
  }
  AUV_BLOCK;

private:
  Block1 m_block_1;
  Block2 m_block_2;
};

template<class Block1, class Block2>
auto operator&(Block1 block1, Block2 block2) {
  using Block1_ = wrap_ref_block<Block1>;
  using Block2_ = wrap_ref_block<Block2>;
  return TeeBlock<Block1_, Block2_>{Block1_{block1}, Block2_{block2}};
}

template<class F>
struct function_type_impl;

template<class T, class R>
struct function_type_impl<R(T)> {
  using arg_type = T;
  using return_type = R;
};

template<class X>
struct function_type_impl<std::function<X>> {
  using arg_type = typename function_type_impl<X>::arg_type;
  using return_type = typename function_type_impl<X>::return_type;
};

template<class F>
using function_arg_type = typename function_type_impl<F>::arg_type;

template<class F>
using function_return_type = typename function_type_impl<F>::return_type;

template<class F, class In = function_arg_type<F>, class Out = function_return_type<F>>
class FunctionBlock : public Block<In, Out> {
public:
  template<class L>
  explicit FunctionBlock(L function) : m_function(function) {}
  Out process(In in) override {
    return m_function(in);
  }

private:
  std::function<Out(In)> m_function;
};

template<typename T>
struct memfun_type {
  using type = void;
};

template<typename Ret, typename Class, typename... Args>
struct memfun_type<Ret (Class::*)(Args...) const> {
  using type = std::function<Ret(Args...)>;
};

template<typename F>
typename memfun_type<decltype(&F::operator())>::type
function_from_lambda(F const &func) {
  return func;
}

template<class F>
FunctionBlock(F) -> FunctionBlock<decltype(function_from_lambda(std::declval<F>()))>;

}// namespace auv

#endif//AUV_HOST_BLOCK_H
