//  Copyright 2018 Odin Holmes.
//            2021 Thomas Figueroa.
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

module;

#if defined(__GNUC__) || defined(__clang__)
#include <concepts>
#include <cstdint>
#endif // defined(__GNUC__ ) || defined(__clang__)

export module Boost.TMP:Algorithm.Zip;

import :Algorithm.MakeSequence;
import :Algorithm.Transform;
import :Algorithm.Unpack;
import :Base.Types;
import :TestHelpers;

#if _MSC_VER
import std;
#endif

// FIXME: zip_ should have a dispatching functionality similar to
// rotate or other large dispatching functions.

namespace boost::tmp {
// zip_ :
export template <typename F, typename C = listify_>
struct zip_ {};

// zip_ : implementation
template<typename F, typename C, typename T, typename U>
struct zip2;
template<typename F, typename C, typename...Ts, typename...Us>
struct zip2<F, C, list_<Ts...>, list_<Us...>> {
    using f = dispatch<find_dispatch(sizeof...(Ts)), C>::template
        f<typename dispatch<2, F>::template f<Ts, Us>...>;
};
template<typename F, typename C, typename T, typename U, typename V>
struct zip3;
template<typename F, typename C, typename... Ts, typename... Us, typename... Vs>
struct zip3<F, C, list_<Ts...>, list_<Us...>, list_<Vs...>> {
    using f = dispatch<find_dispatch(sizeof...(Ts)), C>::template
        f<typename dispatch<3, F>::template f<Ts, Us, Vs>...>;
};
template <typename F, typename C>
struct dispatch<1, zip_<F, C>> : dispatch<1, unpack_<transform_<F, C>>> {};
template <typename F, typename C>
struct dispatch<2, zip_<F, C>> {
    template<typename T, typename U>
    using f = zip2<F, C, T, U>::f;
};
template<typename F, typename C>
struct dispatch<3, zip_<F, C>> {
    template<typename T, typename U, typename V>
    using f = zip3<F, C, T, U, V>::f;
};

// zip_with_index_ :
export template <typename F = listify_, typename C = listify_>
struct zip_with_index_ {};

// zip_with_index_ : implementation
template <typename L>
struct indexer;
template <typename... Is>
struct indexer<list_<Is...>> {
    template <typename F, template<typename...> class C, typename... Ts>
    using f = C<typename dispatch<2, F>::template f<Is, Ts>...>;
};
template <std::size_t N>
using make_index_for_zip = make_seq_impl<next_state(0, N)>::template f<N>;
template <std::size_t N, typename F, typename C>
struct dispatch<N, zip_with_index_<F, C>> {
    template <typename... Ts>
    using f = indexer<make_index_for_zip<sizeof...(Ts)>>::template f<
            F, dispatch<find_dispatch(sizeof...(Ts)), C>::template f, Ts...>;
};
template <std::size_t N, typename F, template <typename...> class C>
struct dispatch<N, zip_with_index_<F, lift_<C>>> {
    template <typename... Ts>
    using f = indexer<make_index_for_zip<sizeof...(Ts)>>::template f<F, C, Ts...>;
};
} // namespace boost::tmp

// TESTING:
namespace zip_test {
using namespace boost::tmp;

template<typename T> requires(std::same_as<T, list_<uint_<3>, uint_<7>>>)
struct AddPairsTogetherWithZip;

// Performs an addition of pairs of elements component wise i.e. (x0 + x1), (y0 + y1)
using test_one = AddPairsTogetherWithZip<call_<zip_<lift_<utils::add>>, list_<int_<1>, int_<3>>, list_<int_<2>, int_<4>>>>;
} // namespace zip_test

// TESTING:
namespace zip_with_index_test {
using namespace boost::tmp;

template<typename T> requires(std::same_as<T, list_<list_<sizet_<0>, char_<'a'>>,
                                                    list_<sizet_<1>, char_<'b'>>,
                                                    list_<sizet_<2>, char_<'c'>>>>)
struct ZipABCWith123;
using test_one = ZipABCWith123<call_<zip_with_index_<>, char_<'a'>, char_<'b'>, char_<'c'>>>;
} // namespace zip_with_index_test