// stolen from  https://gist.github.com/huhlig/8b21850b54a75254be4b093551f8c2cb
// the root is [](https://github.com/rust-lang/rust/issues/44580)
// TODO play with it for some useful example

#include <iostream>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <array>
#include <algorithm>

namespace linAlg {
  template<typename T, unsigned ...dims> class Tensor {
  private:
    template<unsigned N, typename TT, unsigned ...dims1> struct Type;

    template<unsigned N, unsigned dim, unsigned ...dims1, unsigned ...dims2> struct Type<N, std::integer_sequence<unsigned, dim, dims1...>, dims2...> {
      using type = typename Type<N-1, std::integer_sequence<unsigned, dims1...>, dims2..., dim>::type;
    };

    template<unsigned dim, unsigned ...dims1, unsigned ...dims2> struct Type<0, std::integer_sequence<unsigned, dim, dims1...>, dims2...> {
      using type = Tensor<Tensor<T, dim, dims1...>, dims2...>;
    };

    template<unsigned N, typename dims1> struct getDim;

    template<unsigned N, unsigned dim, unsigned ...dims1> struct getDim<N, std::integer_sequence<unsigned, dim, dims1...>> {
      static const unsigned value = getDim<N-1, std::integer_sequence<unsigned, dims1...>>::value;
    };

    template<unsigned dim, unsigned ...dims1> struct getDim<0, std::integer_sequence<unsigned, dim, dims1...>> {
      static const unsigned value = dim;
    };

  public:
    std::array<T, (dims*...)> scalar;

    Tensor() = default;

    Tensor(const Tensor<T, dims...>& tensorIn) {
      for (unsigned i = 0; i < (dims*...); i++)
        scalar[i] = tensorIn.scalar[i];
    }

    Tensor(Tensor<T, dims...>&& tensorIn) {
      for (unsigned i = 0; i < (dims*...); i++)
        scalar[i] = std::move(tensorIn.scalar[i]);
    }

    template<typename ...TT> Tensor(const TT&... args)
    : scalar{args...} {}

    Tensor(std::initializer_list<T> list) {
      std::copy(list.begin(), list.end(), std::inserter(scalar, scalar.begin()));
    }

    template<unsigned ...dims1> Tensor(Tensor<T, dims1...> tensorIn) {
      // TODO: constructor from higher and lower dimensional tensor
    }

    static constexpr unsigned getOrder() {
      return sizeof...(dims);
    }

    template<unsigned N> static constexpr unsigned getNthDim() {
      return getDim<N, std::integer_sequence<unsigned, dims...>>::value;
    }

    template<unsigned dimInt> auto getLower() {
      return typename Type<dimInt, std::integer_sequence<unsigned, dims...>>::type(*this);
    }

    using type = T;
  };

  template<typename T, unsigned dim1> using Vector = Tensor<T, dim1>;
  template<typename T, unsigned dim1, unsigned dim2> using Matrix = Tensor<T, dim1, dim2>;
}

int main() {
  std::cout << linAlg::Tensor<float, 1, 2, 3, 4, 5, 6, 7, 8>().getLower<5>().scalar.at(0).getOrder() << std::endl;
  return 0;
}
