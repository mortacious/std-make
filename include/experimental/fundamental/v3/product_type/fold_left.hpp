//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef JASEL_FUNDAMENTAL_V3_PRODUCT_TYPE_FOLD_LEFT_HPP
#define JASEL_FUNDAMENTAL_V3_PRODUCT_TYPE_FOLD_LEFT_HPP

#include <experimental/fundamental/v3/product_type/product_type.hpp>
#include <utility>

// fixme: fold_let is implementable, as Hana does it, using foldable::apply (hana::unpack).
// Which implementation is more efficient at run-time? at compile time?

namespace std
{
namespace experimental
{
inline  namespace fundamental_v3
{
namespace product_type
{
  namespace detail {

    template <class PT, bool B>
    struct has_at_least_one_element : false_type {};
    template <class PT>
    struct has_at_least_one_element<PT, true> : integral_constant<bool, size_v<PT> >= 1> {};

    template <class T>
    struct drop_front;
    template <class T>
    using drop_front_t = typename drop_front<T>::type;

    template <std::size_t I , std::size_t... Is>
    struct drop_front<index_sequence<I, Is...>> {
      using type = index_sequence<Is...>;
    };

    template <class F, class State, class ProductType>
    constexpr decay_t<State> fold_left_impl( ProductType&& pt, State&& state, F&& f, index_sequence<> )
    {
      return state;
    }

    template <class F, class State, class ProductType, std::size_t I , std::size_t... Is>
    constexpr decltype(auto) fold_left_impl( ProductType&& pt, State&& state, F&& f, index_sequence<I, Is...> )
    {
      return fold_left_impl(
          forward<ProductType>(pt),
          f(forward<State>(state), product_type::get<I>(forward<ProductType>(pt))),
          forward<F>(f),
          index_sequence<Is...>{}
          );
    }

  } // namespace detail

  /**
   * Left-fold of a structure using a binary operation and an optional initial reduction state.
   * fold_left is a left-associative fold using a binary operation.
   * Given a structure containing x1, ..., xn, a function f and an optional initial state, fold_left applies f as follows.
   *  * f(... f(f(f(x1, x2), x3), x4) ..., xn) // without state
   *  * f(... f(f(f(f(state, x1), x2), x3), x4) ..., xn) // with state
   *
   *  When the structure is empty, two things may arise. If an initial state was provided, it is returned as-is.
   *  Otherwise, if the no-state version of the function was used, an error is triggered.
   *  When the stucture contains a single element and the no-state version of the function was used,
   *  that single element is returned as is.
   */

  template <class F, class State, class ProductType
  // todo add constraint on F
  , class = enable_if_t< is_product_type_v<remove_cv_t<remove_reference_t<ProductType>>> >
  >
  constexpr decltype(auto) fold_left(ProductType&& pt, State&& state, F&& f)
  {
    return detail::fold_left_impl(forward<ProductType>(pt), forward<State>(state), forward<F>(f),
          make_index_sequence<product_type::size_v<remove_cv_t<remove_reference_t<ProductType>>>>{});
  }

  template <class F, class ProductType
  // todo add constraint on F
  , class = enable_if_t<
            detail::has_at_least_one_element<
              remove_cv_t<remove_reference_t<ProductType>>,
              is_product_type_v<remove_cv_t<remove_reference_t<ProductType>>>
            >::value
      >
  >
  constexpr decltype(auto) fold_left(ProductType&& pt, F&& f)
  {
    return detail::fold_left_impl(forward<ProductType>(pt), product_type::get<0>(forward<ProductType>(pt)), forward<F>(f),
          detail::drop_front_t<make_index_sequence<product_type::size_v<remove_cv_t<remove_reference_t<ProductType>>>>>{});
  }

}
}}
}

#endif // header
