//  Copyright 2018-2019 Odin Holmes.
//	          2020-2022 Thomas Figueroa.
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

module;

#if defined(__GNUC__) || defined(__clang__)
#include <concepts>
#include <cstdint>
#include <utility>
#endif // defined(__GNUC__ ) || defined(__clang__)

export module Boost.TMP;

#if _MSC_VER
import std;
#endif // _MSC_VER

namespace boost::tmp { // base types

// dispatch : The type finds the appropriate metaclosure to invoke and forms
// the basis of the library.
template <std::size_t N, typename T>
struct dispatch;

// find_dispatch : 
consteval std::size_t find_dispatch(std::size_t n) {
    return n <= 8 ? n :
                    n < 16 ?
                    9 :
                    n == 16 ?
                    16 :
                    n < 32 ?
                    17 :
                    n == 32 ?
                    32 :
                    n < 64 ? 33 : n == 64 ? 64 : n < 128 ? 65 : n == 128 ? 128 : 129;
}

// dispatch_unknown :
template <typename C>
struct dispatch_unknown {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)), C>::template f<Ts...>;
};

consteval std::size_t step_selector(std::size_t N) {
    return N <= 8 ? N : N < 16 ? 8 : N < 32 ? 16 : N < 64 ? 32 : 64;
}

// Boolean type wrapper
export template<bool B>
struct bool_ { static constexpr bool value = B; };

// bool_ type aliases
export using true_  = bool_<true>;
export using false_ = bool_<false>;

// std::byte type wrapper
export template<std::byte B>
struct byte_ { static constexpr std::byte value = B; };

// Char type wrappers
export template<char C>
struct char_ { static constexpr char value = C; };
export template<wchar_t C>
struct wchar_ { static constexpr wchar_t value = C; };
export template<unsigned char C>
struct uchar_ { static constexpr unsigned char value = C; };
export template<char8_t C>
struct char8_ { static constexpr char8_t value = C; };
export template<char16_t C>
struct char16_ { static constexpr char16_t value = C; };
export template<char32_t C>
struct char32_ { static constexpr char32_t value = C; };

// identity_ :
export struct identity_ {};

// identity_ : implementation
template <>
struct dispatch<1, identity_> {
    template <typename T>
    using f = T;
};

// Unsigned and signed integral type wrappers
export template<unsigned long long V>
struct uint_ { static constexpr unsigned long long value = V; };
export template<long long I>
struct int_ { static constexpr long long value = I; };
export template<std::size_t S>
struct sizet_ { static constexpr std::size_t value = S; };
export template<std::uint8_t V>
struct uint8_ { static constexpr std::uint8_t value = V; };
export template<std::uint16_t V>
struct uint16_ { static constexpr std::uint16_t value = V; };
export template<std::uint32_t V>
struct uint32_ { static constexpr std::uint32_t value = V; };
export template<std::uint64_t V>
struct uint64_ { static constexpr std::uint64_t value = V; };
export template<std::int8_t V>
struct int8_ { static constexpr std::int8_t value = V; };
export template<std::int16_t V>
struct int16_ { static constexpr std::int16_t value = V; };
export template<std::int32_t V>
struct int32_ { static constexpr std::int32_t value = V; };
export template<std::int64_t V>
struct int64_ { static constexpr std::int64_t value = V; };

// nothing_ : 
export struct nothing_ {};

// lift_ : Used for lifting a type into a function.
export template <template <typename...> class F, typename C = identity_>
struct lift_ {};

// lift_ : implementation
template <template <typename...> class F, typename C>
struct dispatch<1, lift_<F, C>> {
    template <typename T>
    using f = dispatch<1, C>::template f<F<T>>;
};
template <template <typename...> class F, typename C>
struct dispatch<2, lift_<F, C>> {
    template <typename T0, typename T1>
    using f = dispatch<1, C>::template f<F<T0, T1>>;
};
template <template <typename...> class F, typename C>
struct dispatch<3, lift_<F, C>> {
    template <typename T0, typename T1, typename T2>
    using f = dispatch<1, C>::template f<F<T0, T1, T2>>;
};
template <template <typename...> class F, typename C>
struct dispatch<4, lift_<F, C>> {
    template <typename T0, typename T1, typename T2, typename T3>
    using f = dispatch<1, C>::template f<F<T0, T1, T2, T3>>;
};
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, lift_<F, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<F<Ts...>>;
};

// list_ :
export template <typename... Ts>
struct list_ {};
// listify_ : 
export using listify_ = lift_<list_>;

// always_ : metaclosure returns type / shovels into the continuation C.
export template <typename T, typename C = identity_>
struct always_ {};

// always_ : implementation
template <std::size_t N, typename T, typename C>
struct dispatch<N, always_<T, C>> {
    template <typename...>
    using f = dispatch<1, C>::template f<T>;
};

// result_: The same as always_, but refers to the type inside
// of the input type T.
export template <typename C = identity_>
struct result_ {};

// result_ : implementation
template <typename C>
struct dispatch<1, result_<C>> {
    template <typename T>
    using f = dispatch<1, C>::template f<T::type>;
};

// call_ : a foundational metaclosure that immediately evaluates the input metaclosure(s).
export template <typename F, typename... Ts>
using call_ = dispatch<find_dispatch(sizeof...(Ts)), F>::template f<Ts...>;
export template <typename T, typename... Ts>
using call_t = dispatch<find_dispatch(sizeof...(Ts)), T>::template
                    f<Ts...>::type;
// call_f_ : 
export template <typename C = identity_>
struct call_f_ {};
// call_f_ : implementation
template <std::size_t N, typename C>
struct dispatch<N, call_f_<C>> {
    template <typename F, typename... Ts>
    using f = dispatch<1, C>::template f<
                    typename dispatch<find_dispatch(sizeof...(Ts)), F>::template f<Ts...>>;
};

// maybe_ metaclosure
template <bool B>
struct maybe_test_impl {
    template <typename T>
    using f = T;
};
template <>
struct maybe_test_impl<true> {};
template <typename T> // TODO: Get rid of std::is_same_v here.
using maybe_impl = maybe_test_impl<std::is_same_v<T, nothing_>>::template f<T>;
export template <typename T, typename... Ts>
using maybe_ = maybe_impl<typename dispatch<find_dispatch(sizeof...(Ts)), T>::template
                    f<Ts...>>;
export template <typename T, typename... Ts>
using maybe_t = maybe_impl<typename dispatch<find_dispatch(sizeof...(Ts)), T>::template
                    f<Ts...>::type>;

} // namespace boost::tmp

namespace boost::tmp { // reverse_

// reverse_ :
// Input params: Parameter pack
// Closure params: C - Continuation; default listify_
// Functional description:
// input  -  T0, T1, ..., TN
// apply  - (reverse element order of parameter pack)
// result -  TN, ..., T1, T0
// Empty return type: list_<>
export template <typename C = listify_>
struct reverse_ {};

// reverse_ : implementation
template <typename C, typename... Ts>
struct reverse_impl {
    template <typename... Us>
    using f = dispatch<
        find_dispatch(sizeof...(Ts) + sizeof...(Us)), C>::template f<Ts..., Us...>;
};
template <typename C>
struct dispatch<0, reverse_<C>> {
    template <typename...>
    using f = dispatch<0, C>::template f<>;
};
template <typename C>
struct dispatch<1, reverse_<C>> {
    template <typename T>
    using f = dispatch<1, C>::template f<T>;
};
template <typename C>
struct dispatch<2, reverse_<C>> {
    template <typename T0, typename T1>
    using f = dispatch<2, C>::template f<T1, T0>;
};
template <typename C>
struct dispatch<3, reverse_<C>> {
    template <typename T0, typename T1, typename T2>
    using f = dispatch<3, C>::template f<T2, T1, T0>;
};
template <typename C>
struct dispatch<4, reverse_<C>> {
    template <typename T0, typename T1, typename T2, typename T3>
    using f = dispatch<4, C>::template f<T3, T2, T1, T0>;
};
template <typename C>
struct dispatch<5, reverse_<C>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4>
    using f = dispatch<5, C>::template f<T4, T3, T2, T1, T0>;
};
template <typename C>
struct dispatch<6, reverse_<C>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5>
    using f = dispatch<6, C>::template f<T5, T4, T3, T2, T1, T0>;
};
template <typename C>
struct dispatch<7, reverse_<C>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6>
    using f = dispatch<7, C>::template f<T6, T5, T4, T3, T2, T1, T0>;
};
template <typename C>
struct dispatch<8, reverse_<C>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7>
    using f = dispatch<8, C>::template f<T7, T6, T5, T4, T3, T2, T1, T0>;
};
template <typename C>
struct dispatch<9, reverse_<C>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1),
                        reverse_<lift_<reverse_impl>>>::template f<Ts..., C>::
                            template f<T7, T6, T5, T4, T3, T2, T1, T0>;
};
template <typename C>
struct dispatch<16, reverse_<C>> {
    template <typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
              typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
              typename T10, typename T11, typename T12, typename T13, typename T14,
              typename T15>
    using f = dispatch<16, C>::template f<T15, T14, T13, T12, T11, T10, T9, T8,
                                            T7,  T6,  T5,  T4,  T3,  T2,  T1, T0>;
};
template <typename C>
struct dispatch<17, reverse_<C>> {
    template <typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
              typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
              typename T10, typename T11, typename T12, typename T13, typename T14,
              typename T15, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1),
                                reverse_<lift_<reverse_impl>>>::template f<Ts..., C>::
            template f<T15, T14, T13, T12, T11, T10, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0>;
};
template <typename C>
struct dispatch<32, reverse_<C>> {
    template <typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
              typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
              typename T10, typename T11, typename T12, typename T13, typename T14,
              typename T15, typename T16, typename T17, typename T18, typename T19,
              typename T20, typename T21, typename T22, typename T23, typename T24,
              typename T25, typename T26, typename T27, typename T28, typename T29,
              typename T30, typename T31>
    using f = dispatch<32, C>::template f<T31, T30, T29, T28, T27, T26, T25, T24,
                                          T23, T22, T21, T20, T19, T18, T17, T16,
                                          T15, T14, T13, T12, T11, T10, T9,  T8,
                                          T7,  T6,  T5,  T4,  T3,  T2,  T1,  T0>;
};
template <typename C>
struct dispatch<33, reverse_<C>> {
    template <typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
              typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
              typename T10, typename T11, typename T12, typename T13, typename T14,
              typename T15, typename T16, typename T17, typename T18, typename T19,
              typename T20, typename T21, typename T22, typename T23, typename T24,
              typename T25, typename T26, typename T27, typename T28, typename T29,
              typename T30, typename T31, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1), reverse_<lift_<reverse_impl>>>::template
                f<Ts..., C>::template f<T31, T30, T29, T28, T27, T26, T25, T24,
                                        T23, T22, T21, T20, T19, T18, T17, T16,
                                        T15, T14, T13, T12, T11, T10, T9,  T8,
                                        T7,  T6,  T5,  T4,  T3,  T2,  T1,  T0>;
};
template <typename C>
struct dispatch<64, reverse_<C>> {
    template <typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
              typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
              typename T10, typename T11, typename T12, typename T13, typename T14,
              typename T15, typename T16, typename T17, typename T18, typename T19,
              typename T20, typename T21, typename T22, typename T23, typename T24,
              typename T25, typename T26, typename T27, typename T28, typename T29,
              typename T30, typename T31, typename T32, typename T33, typename T34,
              typename T35, typename T36, typename T37, typename T38, typename T39,
              typename T40, typename T41, typename T42, typename T43, typename T44,
              typename T45, typename T46, typename T47, typename T48, typename T49,
              typename T50, typename T51, typename T52, typename T53, typename T54,
              typename T55, typename T56, typename T57, typename T58, typename T59,
              typename T60, typename T61, typename T62, typename T63>
    using f = dispatch<16, C>::template f<T63, T62, T61, T60, T59, T58, T57, T56,
                                          T55, T54, T53, T52, T51, T50, T49, T48,
                                          T47, T46, T45, T44, T43, T42, T41, T40,
                                          T39, T38, T37, T36, T35, T34, T33, T32,
                                          T31, T30, T29, T28, T27, T26, T25, T24,
                                          T23, T22, T21, T20, T19, T18, T17, T16,
                                          T15, T14, T13, T12, T11, T10, T9,  T8,
                                          T7,  T6,  T5,  T4,  T3,  T2,  T1,  T0>;
};
template <typename C>
struct dispatch<65, reverse_<C>> {
    template <typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
              typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
              typename T10, typename T11, typename T12, typename T13, typename T14,
              typename T15, typename T16, typename T17, typename T18, typename T19,
              typename T20, typename T21, typename T22, typename T23, typename T24,
              typename T25, typename T26, typename T27, typename T28, typename T29,
              typename T30, typename T31, typename T32, typename T33, typename T34,
              typename T35, typename T36, typename T37, typename T38, typename T39,
              typename T40, typename T41, typename T42, typename T43, typename T44,
              typename T45, typename T46, typename T47, typename T48, typename T49,
              typename T50, typename T51, typename T52, typename T53, typename T54,
              typename T55, typename T56, typename T57, typename T58, typename T59,
              typename T60, typename T61, typename T62, typename T63, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1),
                    reverse_<lift_<reverse_impl>>>::template f<Ts..., C>::template
                    f<T63, T62, T61, T60, T59, T58, T57, T56, T55, T54, T53, T52, T51, T50,
                      T49, T48, T47, T46, T45, T44, T43, T42, T41, T40, T39, T38, T37, T36,
                      T35, T34, T33, T32, T31, T30, T29, T28, T27, T26, T25, T24, T23, T22,
                      T21, T20, T19, T18, T17, T16, T15, T14, T13, T12, T11, T10, T9,  T8,
                      T7,  T6,  T5,  T4,  T3,  T2,  T1,  T0>;
};
template <std::size_t N, typename C>
struct dispatch<N, reverse_<C>> : dispatch<65, reverse_<C>> {};

namespace reverse_test {

template<typename T> requires(std::same_as<T, list_<int_<2>, int_<1>, int_<0>>>)
struct ListTwoOneZero;

template<typename T> requires(std::same_as<T, list_<>>)
struct EmptyListReturnsEmptyList;

template<typename T> requires(std::same_as<T, list_<int_<1>>>)
struct SingleElementReturnsListOfSingleElement;

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>, int_<3>>>)
struct DoubleReverseIsOriginal;

using test_one = ListTwoOneZero<call_<reverse_<>, int_<0>, int_<1>, int_<2>>>;

// Reversing empty input results in empty list
using test_two = EmptyListReturnsEmptyList<call_<reverse_<>>>;

// Reversing single input results in single input list
using test_three = SingleElementReturnsListOfSingleElement<call_<reverse_<>, int_<1>>>;

using reverse_test_4 = DoubleReverseIsOriginal<call_<reverse_<reverse_<>>, int_<1>, int_<2>, int_<3>>>;
} // namespace reverse_test

} // namespace boost::tmp

namespace boost::tmp { // drop_ / drop_last_

// drop_ : Remove (N) values from the front of the input VPP.
// Input params: Parameter pack
// Closure params: N - Positive integer type
//                 C - Continuation; default listify_
// Functional description:
// input  - T0, T1, ..., T(M), T(M + 1), ..., TN
// apply  - (drop M values)
// result - T(M), T(M + 1), ..., TN
// Empty return type: list_<>
export template <typename N = sizet_<0>, typename C = listify_>
struct drop_ {};

// drop_ : implementation
template <std::size_t, typename C>
struct drop_impl;
// TODO: Is this correct behavior for dropping nothing?
template <typename C>
struct drop_impl<0, C> {
    template <typename... Ts>
    using f = call_<C, Ts...>;
};
template <typename C>
struct drop_impl<1, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<2, C> {
    template <typename T0, typename T1, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<3, C> {
    template <typename T0, typename T1, typename T2, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<4, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<5, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<6, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<7, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<8, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<16, C> {
    template<typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
             typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
             typename T10, typename T11, typename T12, typename T13, typename T14,
             typename T15, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<32, C> {
    template <typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
              typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
              typename T10, typename T11, typename T12, typename T13, typename T14,
              typename T15, typename T16, typename T17, typename T18, typename T19,
              typename T20, typename T21, typename T22, typename T23, typename T24,
              typename T25, typename T26, typename T27, typename T28, typename T29,
              typename T30, typename T31, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <typename C>
struct drop_impl<64, C> {
    template <typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
              typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
              typename T10, typename T11, typename T12, typename T13, typename T14,
              typename T15, typename T16, typename T17, typename T18, typename T19,
              typename T20, typename T21, typename T22, typename T23, typename T24,
              typename T25, typename T26, typename T27, typename T28, typename T29,
              typename T30, typename T31, typename T32, typename T33, typename T34,
              typename T35, typename T36, typename T37, typename T38, typename T39,
              typename T40, typename T41, typename T42, typename T43, typename T44,
              typename T45, typename T46, typename T47, typename T48, typename T49,
              typename T50, typename T51, typename T52, typename T53, typename T54,
              typename T55, typename T56, typename T57, typename T58, typename T59,
              typename T60, typename T61, typename T62, typename T63, typename... Ts>
    using f = dispatch<sizeof...(Ts), C>::template f<Ts...>;
};
template <std::size_t P, typename C, std::size_t Step = step_selector(P)>
struct make_drop : drop_impl<Step, drop_<sizet_<(P - Step)>, C>> { /* not done */
};
template <std::size_t P, typename C>
struct make_drop<P, C, P> : drop_impl<P, C> {};
template <std::size_t N, typename P, typename C>
struct dispatch<N, drop_<P, C>> : make_drop<P::value, C> {};

export template<typename N = sizet_<0>, typename C = listify_>
struct drop_last_ {};

template<std::size_t N, typename DropN, typename C>
struct dispatch<N, drop_last_<DropN, C>> : dispatch<N, reverse_<drop_<DropN, reverse_<C>>>> {};

namespace drop_test {

template<typename T> requires(std::same_as<T, list_<int_<1>>>)
struct DropOneOffOfList;

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>>>)
struct DropZeroReturnsInputList;

using test_one = DropOneOffOfList<call_<drop_<uint_<1>>, int_<0>, int_<1>>>;

using test_two = DropZeroReturnsInputList<call_<drop_<uint_<0>>, int_<1>, int_<2>>>;

// Dropping off of no input returns empty list
// UNDER CONSIDERATION: Dropping input off of no input fails.
// Should this return an empty list?
// list_<>{} = call_<drop_<uint_<7>>>{};

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>>>)
struct DropThreeOffEnd;

using drop_last_test_1 = DropThreeOffEnd<call_<drop_last_<int_<3>>, int_<1>, int_<2>, int_<3>, int_<4>, int_<5>>>;

} // namespace drop_test

} // namespace boost::tmp

namespace boost::tmp { // rotate_

// rotate_ :
// Input params: Parameter pack
// Closure params: N - Positive (for now) integer type
//                 C - Continuation; default listify_
// Functional description:
// input  - T0, T1, ..., TN
// apply  - (M rotations)
// result - T(M), T(M + 1), ..., T0, T1, ..., TN, ..., T(M - 1)
// Empty return type: list_<>
export template <typename N = sizet_<0>, typename C = listify_>
struct rotate_ {};

// rotate_ : implementation
template <std::size_t, typename C>
struct rotate_impl;
template <typename C>
struct rotate_impl<0, C> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)), C>::template f<Ts...>;
};
template <typename C>
struct rotate_impl<1, C> {
    template <typename T, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1), C>::template
                    f<Ts..., T>;
};
template <typename C>
struct rotate_impl<2, C> {
    template <typename T0, typename T1, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 2), C>::template
                    f<Ts..., T0, T1>;
};
template <typename C>
struct rotate_impl<3, C> {
    template <typename T0, typename T1, typename T2, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 3), C>::template
                    f<Ts..., T0, T1, T2>;
};
template <typename C>
struct rotate_impl<4, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 4), C>::template
                    f<Ts..., T0, T1, T2, T3>;
};
template <typename C>
struct rotate_impl<5, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 5), C>::template
                    f<Ts..., T0, T1, T2, T3, T4>;
};
template <typename C>
struct rotate_impl<6, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 6), C>::template
                    f<Ts..., T0, T1, T2, T3, T4, T5>;
};
template <typename C>
struct rotate_impl<7, C> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 7), C>::template
                    f<Ts..., T0, T1, T2, T3, T4, T5, T6>;
};
template <typename C>
struct rotate_impl<8, C> {
    template<typename T0, typename T1, typename T2, typename T3, typename T4,
             typename T5, typename T6, typename T7, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 8), C>::template
                    f<Ts..., T0, T1, T2, T3, T4, T5, T6, T7>;
};
template <typename C>
struct rotate_impl<16, C> {
    template<typename T0, typename T1, typename T2, typename T3, typename T4,
             typename T5, typename T6, typename T7, typename T8, typename T9,
             typename T10, typename T11, typename T12, typename T13, typename T14,
             typename T15, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 16), C>::template
                    f<Ts..., T0, T1, T2,  T3,  T4,  T5,  T6,  T7,
                             T8, T9, T10, T11, T12, T13, T14, T15>;
};
template <typename C>
struct rotate_impl<32, C> {
    template<typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
             typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
             typename T10, typename T11, typename T12, typename T13, typename T14,
             typename T15, typename T16, typename T17, typename T18, typename T19,
             typename T20, typename T21, typename T22, typename T23, typename T24,
             typename T25, typename T26, typename T27, typename T28, typename T29,
             typename T30, typename T31, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 32), C>::template
                    f<Ts..., T0,  T1,  T2,  T3,  T4,  T5,  T6,  T7,
                             T8,  T9,  T10, T11, T12, T13, T14, T15,
                             T16, T17, T18, T19, T20, T21, T22, T23,
                             T24, T25, T26, T27, T28, T29, T30, T31>;
};
template <typename C>
struct rotate_impl<64, C> {
    template<typename T0,  typename T1,  typename T2,  typename T3,  typename T4,
             typename T5,  typename T6,  typename T7,  typename T8,  typename T9,
             typename T10, typename T11, typename T12, typename T13, typename T14,
             typename T15, typename T16, typename T17, typename T18, typename T19,
             typename T20, typename T21, typename T22, typename T23, typename T24,
             typename T25, typename T26, typename T27, typename T28, typename T29,
             typename T30, typename T31, typename T32, typename T33, typename T34,
             typename T35, typename T36, typename T37, typename T38, typename T39,
             typename T40, typename T41, typename T42, typename T43, typename T44,
             typename T45, typename T46, typename T47, typename T48, typename T49,
             typename T50, typename T51, typename T52, typename T53, typename T54,
             typename T55, typename T56, typename T57, typename T58, typename T59,
             typename T60, typename T61, typename T62, typename T63, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 64), C>::template
                    f<Ts..., T0,  T1,  T2,  T3,  T4,  T5,  T6,  T7,  T8,  T9,  T10,
                             T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21,
                             T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32,
                             T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43,
                             T44, T45, T46, T47, T48, T49, T50, T51, T52, T53, T54,
                             T55, T56, T57, T58, T59, T60, T61, T62, T63>;
};
template <typename P, typename C>
struct dispatch<0, rotate_<P, C>> {
    template <typename...>
    using f = dispatch<0, C>::template f<>;
};
template <std::size_t P, typename C, std::size_t Step = step_selector(P)>
struct make_rotate
    : rotate_impl<step_selector(Step), rotate_<sizet_<(P - Step)>, C>> { /* not done */
};
template <std::size_t P, typename C>
struct make_rotate<P, C, P> : rotate_impl<P, C> {};
template <std::size_t N, typename P, typename C>
struct dispatch<N, rotate_<P, C>> : make_rotate<P::value, C> {};

namespace rotate_test {

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>, int_<3>>>)
struct ZeroRotation;

template<typename T> requires(std::same_as<T, list_<int_<2>, int_<3>, int_<1>>>)
struct DoOneRotation;

template<typename T> requires(std::same_as<T, list_<>>)
struct RotateEmptyList;

using test_one   = ZeroRotation<call_<rotate_<int_<0>>, int_<1>, int_<2>, int_<3>>>;

using test_two   = DoOneRotation<call_<rotate_<int_<1>>, int_<1>, int_<2>, int_<3>>>;

using test_three = RotateEmptyList<call_<rotate_<int_<0>>>>;

// TODO: Allow rotations in opposite direction.
// list_<int_<3>, int_<1>, int_<2>>{} = call_<rotate_<int_<-1>>, int_<1>, int_<2>, int_<3>>{};
} // namespace rotate_test

} // namespace boost::tmp

namespace boost::tmp { // filter_
// filter_ :
export template <typename F, typename C = listify_>
struct filter_ {};

// filter_ : implementation
template <std::size_t N, template <typename...> class F, typename C>
struct filtery;
template <template <typename...> class F, typename C>
struct filtery<0, F, C> {
    template <std::size_t N, typename T, typename U, typename... Ts>
    using f = filtery<(F<U>::value + 2 * (N == 1)), F,
                                C>::template f<(N - 1), U, Ts...>;
};
template <template <typename...> class F, typename C>
struct filtery<1, F, C> {
    template <std::size_t N, typename T, typename U, typename... Ts>
    using f = filtery<(F<U>::value + 2 * (N == 1)), F, C>::template
                    f<(N - 1), U, Ts..., T>;
};
template <template <typename...> class F, typename C>
struct filtery<2, F, C> { // all the way around, remove last
    template <std::size_t N, typename T, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)), C>::template f<Ts...>;
};
template <template <typename...> class F, typename C>
struct filtery<3, F, C> { // all the way around, keep last
    template <std::size_t N, typename T, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1), C>::template f<Ts..., T>;
};
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, filter_<lift_<F>, C>> {
    template <typename T, typename... Ts>
    using f = filtery<(F<T>::value + 2 * (sizeof...(Ts) == 0)), F,
                                C>::template f<(sizeof...(Ts)), T, Ts...>;
};
template <std::size_t N, typename F, typename C>
struct dispatch<N, filter_<F, C>> {
    template <typename T, typename... Ts>
    using f = filtery<
            (dispatch<1, F>::template f<T>::value + 2 * (sizeof...(Ts) == 0)),
            dispatch<1, F>::template f, C>::template f<(sizeof...(Ts)), T, Ts...>;
};
template <typename F, typename C>
struct dispatch<0, filter_<F, C>> {
    template <typename... Ts>
    using f = dispatch<0, C>::template f<>;
};
template <template <typename...> class F, typename C>
struct dispatch<0, filter_<lift_<F>, C>> {
    template <typename... Ts>
    using f = dispatch<0, C>::template f<>;
};

namespace filter_test {

template<typename T> requires(std::same_as<T, list_<int_<2>, int_<4>, int_<6>, int_<8>, int_<10>>>)
struct FilterOutOddNumbers;

template<typename T>
using is_even = bool_<(T::value % 2 == 0)>;

using test_one = FilterOutOddNumbers<call_<filter_<lift_<is_even>>,
            int_<1>, int_<2>, int_<3>, int_<4>, int_<5>,
            int_<6>, int_<7>, int_<8>, int_<9>, int_<10>>>;

} // namespace filter_test

} // namespace boost::tmp

namespace boost::tmp { // join_ / join_seq_

export template <typename C = listify_>
struct join_ {};

export template<typename C = listify_>
struct join_seq_ {};

template<std::size_t... Is>
using IndexSeq = std::index_sequence<Is...>;

export template<typename... Ts>
using into_sequence = IndexSeq<Ts::value...>;

template <template <typename...> class C, typename...>
struct joiner;
template <template <typename...> class C,
            typename... T0s, typename... T1s, typename... T2s, typename... T3s,
            typename... T4s, typename... T5s, typename... T6s, typename... T7s>
struct joiner<C, list_<T0s...>, list_<T1s...>, list_<T2s...>, list_<T3s...>,
                    list_<T4s...>, list_<T5s...>, list_<T6s...>, list_<T7s...>> {
    template <typename... Vs>
    using f = C<T0s..., T1s..., T2s..., T3s..., T4s..., T5s..., T6s..., T7s..., Vs...>;
};
template<template <typename...> class C,
         typename... T0s,  typename... T1s,  typename... T2s,  typename... T3s,
         typename... T4s,  typename... T5s,  typename... T6s,  typename... T7s,
         typename... T8s,  typename... T9s,  typename... T10s, typename... T11s,
         typename... T12s, typename... T13s, typename... T14s, typename... T15s,
         typename... T16s, typename... T17s, typename... T18s, typename... T19s,
         typename... T20s, typename... T21s, typename... T22s, typename... T23s,
         typename... T24s, typename... T25s, typename... T26s, typename... T27s,
         typename... T28s, typename... T29s, typename... T30s, typename... T31s>
struct joiner<C, list_<T0s...>,  list_<T1s...>,  list_<T2s...>,  list_<T3s...>,
                 list_<T4s...>,  list_<T5s...>,  list_<T6s...>,  list_<T7s...>,
                 list_<T8s...>,  list_<T9s...>,  list_<T10s...>, list_<T11s...>,
                 list_<T12s...>, list_<T13s...>, list_<T14s...>, list_<T15s...>,
                 list_<T16s...>, list_<T17s...>, list_<T18s...>, list_<T19s...>,
                 list_<T20s...>, list_<T21s...>, list_<T22s...>, list_<T23s...>,
                 list_<T24s...>, list_<T25s...>, list_<T26s...>, list_<T27s...>,
                 list_<T28s...>, list_<T29s...>, list_<T30s...>, list_<T31s...>> {
    template <typename... Vs>
    using f = C<T0s...,  T1s...,  T2s...,  T3s...,  T4s...,  T5s...,  T6s...,  T7s...,
                T8s...,  T9s...,  T10s..., T11s..., T12s..., T13s..., T14s..., T15s...,
                T16s..., T17s..., T18s..., T19s..., T20s..., T21s..., T22s..., T23s...,
                T24s..., T25s..., T26s..., T27s..., T28s..., T29s..., T30s..., T31s..., Vs...>;
};
template <std::size_t N>
struct join_loop;
template <>
struct join_loop<1> {
    template <template <typename...> class C,
              typename T0  = list_<>, typename T1  = list_<>, typename T2  = list_<>,
              typename T3  = list_<>, typename T4  = list_<>, typename T5  = list_<>,
              typename T6  = list_<>, typename T7  = list_<>, typename T8  = list_<>,
              typename T9  = list_<>, typename T10 = list_<>, typename T11 = list_<>,
              typename T12 = list_<>, typename T13 = list_<>, typename T14 = list_<>,
              typename T15 = list_<>, typename T16 = list_<>, typename T17 = list_<>,
              typename T18 = list_<>, typename T19 = list_<>, typename T20 = list_<>,
              typename T21 = list_<>, typename T22 = list_<>, typename T23 = list_<>,
              typename T24 = list_<>, typename T25 = list_<>, typename T26 = list_<>,
              typename T27 = list_<>, typename T28 = list_<>, typename T29 = list_<>,
              typename T30 = list_<>, typename T31 = list_<>, typename... Ts>
    using f = join_loop<(sizeof...(Ts) > 8)>::template f<
            joiner<C, T0,  T1,  T2,  T3,  T4,  T5,  T6,  T7,  T8,  T9,  T10, T11, T12, T13,
                      T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27,
                      T28, T29, T30, T31>::template f, Ts...>;
};
template <>
struct join_loop<0> {
    template <template <typename...> class C,
              typename T0 = list_<>, typename T1 = list_<>, typename T2 = list_<>,
              typename T3 = list_<>, typename T4 = list_<>, typename T5 = list_<>,
              typename T6 = list_<>, typename T7 = list_<>, typename T8 = list_<>>
    using f = joiner<C, T0, T1, T2, T3, T4, T5, T6, T7>::template f<>;
};
template <std::size_t N, template <typename...> class C>
struct dispatch<N, join_<lift_<C>>> {
    template <typename... Ts>
    using f = join_loop<(sizeof...(Ts) > 8)>::template f<C, Ts...>;
};
template <std::size_t N, typename C>
struct dispatch<N, join_<C>> {
    template <typename... Ts>
    using f = join_loop<(
            sizeof...(Ts) > 8)>::template f<dispatch_unknown<C>::template f, Ts...>;
};

// Sequence joining metaclosures
template <template <typename...> class C, typename...>
struct seq_joiner;
template <template <typename...> class C, std::size_t... T0s, std::size_t... T1s,
            std::size_t... T2s, std::size_t... T3s, std::size_t... T4s, std::size_t... T5s,
            std::size_t... T6s, std::size_t... T7s>
struct seq_joiner<C, IndexSeq<T0s...>, IndexSeq<T1s...>, IndexSeq<T2s...>, IndexSeq<T3s...>,
                IndexSeq<T4s...>, IndexSeq<T5s...>, IndexSeq<T6s...>, IndexSeq<T7s...>> {
    template <typename... Vs>
    using f = C<sizet_<T0s>..., sizet_<T1s>..., sizet_<T2s>..., sizet_<T3s>..., sizet_<T4s>..., sizet_<T5s>..., sizet_<T6s>..., sizet_<T7s>..., Vs...>;
};
template <template <typename...> class C,
          std::size_t... T0s,  std::size_t... T1s,  std::size_t... T2s,  std::size_t... T3s,
          std::size_t... T4s,  std::size_t... T5s,  std::size_t... T6s,  std::size_t... T7s,
          std::size_t... T8s,  std::size_t... T9s,  std::size_t... T10s, std::size_t... T11s,
          std::size_t... T12s, std::size_t... T13s, std::size_t... T14s, std::size_t... T15s,
          std::size_t... T16s, std::size_t... T17s, std::size_t... T18s, std::size_t... T19s,
          std::size_t... T20s, std::size_t... T21s, std::size_t... T22s, std::size_t... T23s,
          std::size_t... T24s, std::size_t... T25s, std::size_t... T26s, std::size_t... T27s,
          std::size_t... T28s, std::size_t... T29s, std::size_t... T30s, std::size_t... T31s>
struct seq_joiner<C, IndexSeq<T0s...>,  IndexSeq<T1s...>,  IndexSeq<T2s...>,  IndexSeq<T3s...>,
                     IndexSeq<T4s...>,  IndexSeq<T5s...>,  IndexSeq<T6s...>,  IndexSeq<T7s...>,
                     IndexSeq<T8s...>,  IndexSeq<T9s...>,  IndexSeq<T10s...>, IndexSeq<T11s...>,
                     IndexSeq<T12s...>, IndexSeq<T13s...>, IndexSeq<T14s...>, IndexSeq<T15s...>,
                     IndexSeq<T16s...>, IndexSeq<T17s...>, IndexSeq<T18s...>, IndexSeq<T19s...>,
                     IndexSeq<T20s...>, IndexSeq<T21s...>, IndexSeq<T22s...>, IndexSeq<T23s...>,
                     IndexSeq<T24s...>, IndexSeq<T25s...>, IndexSeq<T26s...>, IndexSeq<T27s...>,
                     IndexSeq<T28s...>, IndexSeq<T29s...>, IndexSeq<T30s...>, IndexSeq<T31s...>> {
    template <typename... Vs>
    using f = C<sizet_<T0s>...,  sizet_<T1s>...,  sizet_<T2s>...,  sizet_<T3s>...,
                sizet_<T4s>...,  sizet_<T5s>...,  sizet_<T6s>...,  sizet_<T7s>...,
                sizet_<T8s>...,  sizet_<T9s>...,  sizet_<T10s>..., sizet_<T11s>...,
                sizet_<T12s>..., sizet_<T13s>..., sizet_<T14s>..., sizet_<T15s>...,
                sizet_<T16s>..., sizet_<T17s>..., sizet_<T18s>..., sizet_<T19s>...,
                sizet_<T20s>..., sizet_<T21s>..., sizet_<T22s>..., sizet_<T23s>...,
                sizet_<T24s>..., sizet_<T25s>..., sizet_<T26s>..., sizet_<T27s>...,
                sizet_<T28s>..., sizet_<T29s>..., sizet_<T30s>..., sizet_<T31s>..., Vs...>;
};
template <std::size_t N>
struct seq_join_loop;
template <>
struct seq_join_loop<1> {
    template <template <typename...> class C,
              typename T0  = IndexSeq<>, typename T1  = IndexSeq<>, typename T2  = IndexSeq<>,
              typename T3  = IndexSeq<>, typename T4  = IndexSeq<>, typename T5  = IndexSeq<>,
              typename T6  = IndexSeq<>, typename T7  = IndexSeq<>, typename T8  = IndexSeq<>,
              typename T9  = IndexSeq<>, typename T10 = IndexSeq<>, typename T11 = IndexSeq<>,
              typename T12 = IndexSeq<>, typename T13 = IndexSeq<>, typename T14 = IndexSeq<>,
              typename T15 = IndexSeq<>, typename T16 = IndexSeq<>, typename T17 = IndexSeq<>,
              typename T18 = IndexSeq<>, typename T19 = IndexSeq<>, typename T20 = IndexSeq<>,
              typename T21 = IndexSeq<>, typename T22 = IndexSeq<>, typename T23 = IndexSeq<>,
              typename T24 = IndexSeq<>, typename T25 = IndexSeq<>, typename T26 = IndexSeq<>,
              typename T27 = IndexSeq<>, typename T28 = IndexSeq<>, typename T29 = IndexSeq<>,
              typename T30 = IndexSeq<>, typename T31 = IndexSeq<>, typename... Ts>
    using f = seq_join_loop<(sizeof...(Ts) > 8)>::template f<
            seq_joiner<C, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14,
                          T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27,
                          T28, T29, T30, T31>::template f, Ts...>;
};
template <>
struct seq_join_loop<0> {
    template <template <typename...> class C,
              typename T0 = IndexSeq<>, typename T1 = IndexSeq<>, typename T2 = IndexSeq<>,
              typename T3 = IndexSeq<>, typename T4 = IndexSeq<>, typename T5 = IndexSeq<>,
              typename T6 = IndexSeq<>, typename T7 = IndexSeq<>, typename T8 = IndexSeq<>>
    using f = seq_joiner<C, T0, T1, T2, T3, T4, T5, T6, T7>::template f<>;
};
template <std::size_t N, template <typename...> class C>
struct dispatch<N, join_seq_<lift_<C>>> {
    template <typename... Ts>
    using f = seq_join_loop<(sizeof...(Ts) > 8)>::template f<C, Ts...>;
};
template <std::size_t N, typename C>
struct dispatch<N, join_seq_<C>> {
    template <typename... Ts>
    using f = seq_join_loop<(sizeof...(Ts) > 8)>::template
                f<dispatch_unknown<C>::template f, Ts...>;
};

} // namespace boost::tmp

namespace boost::tmp { // size_

// size_ :
// Input params: Parameter pack
// Closure params: C - Continuation; default identity_
// Functional description:
// input  - T0, T1, ..., TN (Ts...)
// apply  - (get number of elements in parameter pack)
// result - sizet_<sizeof...(Ts)>
// Empty return type: sizet_<0>
export template <typename C = identity_>
struct size_ {};

// size_ : implementation
template <std::size_t N, typename C>
struct dispatch<N, size_<C>> {
    template <typename... Ls>
    using f = dispatch<1, C>::template f<sizet_<sizeof...(Ls)>>;
};

namespace size_test {

template<typename T> requires(std::same_as<T, sizet_<3>>)
struct ThreeElementsInPack;

template<typename T> requires(std::same_as<T, sizet_<0>>)
struct EmptyPackIsZero;

using test_one = ThreeElementsInPack<call_<size_<>, int_<0>, int_<2>, int_<4>>>;

// No input list is zero size.
using test_two = EmptyPackIsZero<call_<size_<>>>;

} // namespace size_test

} // namespace boost::tmp

namespace boost::tmp { // fold_left_ / fold_right_

/// fold_left_ : folds left over a list using a binary predicate /
/// fold left considers the first element in the input pack the initial state, use
/// `push_front_<>` to add initial state if needed
/// Multiple input, single output

// fold_left_ : 
export template <typename F = listify_, typename C = identity_>
struct fold_left_ {};

// fold_left_ : implementation
template <template <typename...> class F, template <typename...> class C>
struct dispatch<0, fold_left_<lift_<F>, lift_<C>>> {
    template <typename...>
    using f = nothing_;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<1, fold_left_<lift_<F>, lift_<C>>> {
    template <typename T>
    using f = C<T>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<2, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In, typename T0>
    using f = C<F<In, T0>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<3, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In, typename T0, typename T1>
    using f = C<F<F<In, T0>, T1>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<4, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In, typename T0, typename T1, typename T2>
    using f = C<F<F<F<In, T0>, T1>, T2>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<5, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In, typename T0, typename T1, typename T2, typename T3>
    using f = C<F<F<F<F<In, T0>, T1>, T2>, T3>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<6, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In, typename T0, typename T1, typename T2, typename T3,
              typename T4>
    using f = C<F<F<F<F<F<In, T0>, T1>, T2>, T3>, T4>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<7, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In, typename T0, typename T1, typename T2, typename T3,
              typename T4, typename T5>
    using f = C<F<F<F<F<F<F<In, T0>, T1>, T2>, T3>, T4>, T5>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<8, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In, typename T0, typename T1, typename T2, typename T3,
              typename T4, typename T5, typename T6>
    using f = C<F<F<F<F<F<F<F<In, T0>, T1>, T2>, T3>, T4>, T5>, T6>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<9, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In, typename T0, typename T1, typename T2, typename T3,
              typename T4, typename T5, typename T6, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1),
                                fold_left_<lift_<F>, lift_<C>>>::
            template f<F<F<F<F<F<F<F<In, T0>, T1>, T2>, T3>, T4>, T5>, T6>, Ts...>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<16, fold_left_<lift_<F>, lift_<C>>> {
    template <typename In,  typename T0,  typename T1,  typename T2,  typename T3,
              typename T4,  typename T5,  typename T6,  typename T7,  typename T8,
              typename T9,  typename T10, typename T11, typename T12, typename T13,
              typename T14, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1), fold_left_<lift_<F>, lift_<C>>>::template
                  f<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<In,
                                                  T0>,
                                                T1>,
                                              T2>,
                                            T3>,
                                          T4>,
                                        T5>,
                                      T6>,
                                    T7>,
                                  T8>,
                                T9>,
                              T10>,
                            T11>,
                          T12>,
                        T13>,
                      T14>,
                    Ts...>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<17, fold_left_<lift_<F>, lift_<C>>>
    : dispatch<16, fold_left_<lift_<F>, lift_<C>>> {};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<32, fold_left_<lift_<F>, lift_<C>>> {
    template<typename In,  typename T0,  typename T1,  typename T2,  typename T3,
             typename T4,  typename T5,  typename T6,  typename T7,  typename T8,
             typename T9,  typename T10, typename T11, typename T12, typename T13,
             typename T14, typename T15, typename T16, typename T17, typename T18,
             typename T19, typename T20, typename T21, typename T22, typename T23,
             typename T24, typename T25, typename T26, typename T27, typename T28,
             typename T29, typename T30, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1),
                fold_left_<lift_<F>, lift_<C>>>::template
                  f<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<In,
                                                                                  T0>,
                                                                                T1>,
                                                                              T2>,
                                                                            T3>,
                                                                          T4>,
                                                                        T5>,
                                                                      T6>,
                                                                    T7>,
                                                                  T8>,
                                                                T9>,
                                                              T10>,
                                                            T11>,
                                                          T12>,
                                                        T13>,
                                                      T14>,
                                                    T15>,
                                                  T16>,
                                                T17>,
                                              T18>,
                                            T19>,
                                          T20>,
                                        T21>,
                                      T22>,
                                    T23>,
                                  T24>,
                                T25>,
                              T26>,
                            T27>,
                          T28>,
                        T29>,
                      T30>,
                    Ts...>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<33, fold_left_<lift_<F>, lift_<C>>>
    : dispatch<32, fold_left_<lift_<F>, lift_<C>>> {};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<64, fold_left_<lift_<F>, lift_<C>>> {
    template<typename In,  typename T0,  typename T1,  typename T2,  typename T3,
             typename T4,  typename T5,  typename T6,  typename T7,  typename T8,
             typename T9,  typename T10, typename T11, typename T12, typename T13,
             typename T14, typename T15, typename T16, typename T17, typename T18,
             typename T19, typename T20, typename T21, typename T22, typename T23,
             typename T24, typename T25, typename T26, typename T27, typename T28,
             typename T29, typename T30, typename T31, typename T32, typename T33,
             typename T34, typename T35, typename T36, typename T37, typename T38,
             typename T39, typename T40, typename T41, typename T42, typename T43,
             typename T44, typename T45, typename T46, typename T47, typename T48,
             typename T49, typename T50, typename T51, typename T52, typename T53,
             typename T54, typename T55, typename T56, typename T57, typename T58,
             typename T59, typename T60, typename T61, typename T62, typename T63,
             typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1),
            fold_left_<lift_<F>, lift_<C>>>::
    template f<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<F<In,
                                                                                                                                                T0>,
                                                                                                                                              T1>,
                                                                                                                                            T2>,
                                                                                                                                          T3>,
                                                                                                                                        T4>,
                                                                                                                                      T5>,
                                                                                                                                    T6>,
                                                                                                                                  T7>,
                                                                                                                                T8>,
                                                                                                                              T9>,
                                                                                                                            T10>,
                                                                                                                          T11>,
                                                                                                                        T12>,
                                                                                                                      T13>,
                                                                                                                    T14>,
                                                                                                                  T15>,
                                                                                                                T16>,
                                                                                                              T17>,
                                                                                                            T18>,
                                                                                                          T19>,
                                                                                                        T20>,
                                                                                                      T21>,
                                                                                                    T22>,
                                                                                                  T23>,
                                                                                                T24>,
                                                                                              T25>,
                                                                                            T26>,
                                                                                          T27>,
                                                                                        T28>,
                                                                                      T29>,
                                                                                    T30>,
                                                                                  T31>,
                                                                                T32>,
                                                                              T33>,
                                                                            T34>,
                                                                          T35>,
                                                                        T36>,
                                                                      T37>,
                                                                    T38>,
                                                                  T39>,
                                                                T40>,
                                                              T41>,
                                                            T42>,
                                                          T43>,
                                                        T44>,
                                                      T45>,
                                                    T46>,
                                                  T47>,
                                                T48>,
                                              T49>,
                                            T50>,
                                          T51>,
                                        T52>,
                                      T53>,
                                    T54>,
                                  T55>,
                                T56>,
                              T57>,
                            T58>,
                          T59>,
                        T60>,
                      T61>,
                    T62>,
                  T63>,
                Ts...>;
};
template <std::size_t N, template <typename...> class F, template <typename...> class C>
struct dispatch<N, fold_left_<lift_<F>, lift_<C>>>
    : dispatch<64, fold_left_<lift_<F>, lift_<C>>> {};
template <std::size_t N, typename F, typename C>
struct dispatch<N, fold_left_<F, C>>
    : dispatch<N, fold_left_<lift_<dispatch<2, F>::template f>,
                                lift_<dispatch<1, C>::template f>>> {};
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, fold_left_<lift_<F>, C>>
    : dispatch<N, fold_left_<lift_<F>, lift_<dispatch<1, C>::template f>>> {};
template <std::size_t N, typename F, template <typename...> class C>
struct dispatch<N, fold_left_<F, lift_<C>>>
    : dispatch<N, fold_left_<lift_<dispatch<2, F>::template f>, lift_<C>>> {};

// fold_right_ : folds right over a list using a binary predicate
// fold right considers the last element in the input pack the initial state, use
// push_back to add initial state if needed
export template <typename F, typename C = identity_>
struct fold_right_ {};

// fold_right_ : implementation
template <template <typename...> class F, template <typename...> class C>
struct dispatch<0, fold_right_<lift_<F>, lift_<C>>> {
    template <typename...>
    using f = nothing_;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<1, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T>
    using f = C<T>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<2, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T0, typename In>
    using f = C<F<In, T0>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<3, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T0, typename T1, typename In>
    using f = C<F<F<In, T1>, T0>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<4, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T0, typename T1, typename T2, typename In>
    using f = C<F<F<F<In, T2>, T1>, T0>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<5, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T0, typename T1, typename T2, typename T3, typename In>
    using f = C<F<F<F<F<In, T3>, T2>, T1>, T0>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<6, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename In>
    using f = C<F<F<F<F<F<In, T4>, T3>, T2>, T1>, T0>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<7, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename In>
    using f = C<F<F<F<F<F<F<In, T5>, T4>, T3>, T2>, T1>, T0>>;
};
template <template <typename...> class F, template <typename...> class C>
struct dispatch<8, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename In>
    using f = C<F<F<F<F<F<F<F<In, T6>, T5>, T4>, T3>, T2>, T1>, T0>>;
};
template <std::size_t N, template <typename...> class F, template <typename...> class C>
struct dispatch<N, fold_right_<lift_<F>, lift_<C>>> {
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7, typename T8, typename... Ts>
    using f = C<F<F<F<F<F<F<F<F<F<typename dispatch<find_dispatch(sizeof...(Ts)),
                                        fold_right_<lift_<F>, identity_>>::template f<Ts...>,
                                T8>,
                              T7>,
                            T6>,
                          T5>,
                        T4>,
                      T3>,
                    T2>,
                  T1>,
                T0>>;
};
template <std::size_t N, typename F, typename C>
struct dispatch<N, fold_right_<F, C>>
    : dispatch<N, fold_right_<lift_<dispatch<2, F>::template f>,
                                lift_<dispatch<1, C>::template f>>> {};
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, fold_right_<lift_<F>, C>>
    : dispatch<N, fold_right_<lift_<F>, lift_<dispatch<1, C>::template f>>> {};
template <std::size_t N, typename F, template <typename...> class C>
struct dispatch<N, fold_right_<F, lift_<C>>>
    : dispatch<N, fold_right_<lift_<dispatch<2, F>::template f>, lift_<C>>> {};

namespace fold_tests {

template<typename T> requires(std::same_as<T, list_<>>)
struct FoldLeftIntoEmptyList;

template<typename T> requires(std::same_as<T, list_<uint_<1>, uint_<2>>>)
struct FoldLeftValuesIntoList;

template <typename T, typename U>
using pusher = call_<join_<>, T, list_<U>>;

// Returns an empty list.
using fold_left_test_1 = FoldLeftIntoEmptyList<call_<fold_left_<lift_<pusher>>, list_<>>>;

// Returns list of folded over values uint_<1> and uint_<2>.
using fold_left_test_2 = FoldLeftValuesIntoList<call_<fold_left_<lift_<pusher>>, list_<>, uint_<1>, uint_<2>>>;

using namespace boost::tmp;

template<typename T> requires(std::same_as<T, uint_<20>>)
struct AddsUpToTwenty;

template <typename T, typename U>
using add = uint_<(T::value + U::value)>;

using fold_right_test_1 = AddsUpToTwenty<call_<fold_right_<lift_<add>>, uint_<1>, uint_<10>, uint_<9>>>;

} // namespace fold_tests

} // namespace boost::tmp - fold_left_ / fold_right_

namespace boost::tmp { // unpack_

// \brief turns a list of types into a variadic pack of those types /
// example: call<all<>,true_,false_,true_> is equivalent to
// call<unpack<all<>>,list<true_,false_,true_>>
// \requirement
// Unpack always needs a continuation, so even if you're just unpacking
// a list, you need to use it like the following:
// using alist = list_<uint_<0>>;
// uint_<0>{} = call_<unpack_<identity_>, alist>{}; // This will be vaild
export template <typename C>
struct unpack_ {};

template <typename C, typename L>
struct unpack_impl;
template <typename C, template <typename...> class Seq, typename... Ls>
struct unpack_impl<C, Seq<Ls...>> {
    using type = dispatch<find_dispatch(sizeof...(Ls)), C>::template f<Ls...>;
};

// in case of nothing_ input give a nothing_ output
template <typename C>
struct unpack_impl<C, nothing_> {
    using type = nothing_;
};

template <typename C>
struct dispatch<1, unpack_<C>> {
    template <typename L>
    using f = unpack_impl<C, L>::type;
};

namespace unpack_test {

} // namespace unpack_test

} // namespace boost::tmp

namespace boost::tmp { // flatten_

// flatten_ : 
export template <typename C = listify_>
struct flatten_ {};

// flatten_ : implementation
template <typename C, typename... Ts>
struct flatten_impl {
    using type = dispatch<0, join_<C>>::template f<Ts...>;
};
// Append to list function
template <typename C, typename... T1s, typename X, typename... T2s>
struct flatten_impl<C, list_<T1s...>, X, T2s...>
    : flatten_impl<C, list_<T1s..., X>, T2s...> {};
// Remove from list function
template <typename C, typename... T1s, template <typename...> class X, typename... T2s,
            typename... T3s>
struct flatten_impl<C, list_<T1s...>, X<T2s...>, T3s...>
    : flatten_impl<C, list_<T1s...>, T2s..., T3s...> {};
template <std::size_t N, typename C>
struct dispatch<N, flatten_<C>> {
    template <typename... Ts>
    using f = flatten_impl<C, list_<>, Ts...>::type;
};

namespace flatten_test {

template<typename T> requires(std::same_as<T, list_<uint_<0>, uint_<1>, uint_<2>, uint_<3>, uint_<4>>>)
struct SingleFlattenedList;

using test_one = SingleFlattenedList<call_<flatten_<>, list_<list_<uint_<0>, uint_<1>>, uint_<2>, list_<uint_<3>>, uint_<4>>>>;

} // namespace flatten_test

} // namespace boost::tmp

namespace boost::tmp { // index_ / unpack_index_ / i(N)_ / ui(N)_

export template<typename I, typename C = identity_>
struct index_ {};
export template<typename I, typename C = identity_>
using unpack_index_ = unpack_<index_<I, C>>;
export template<typename C = identity_>
using front_ = index_<sizet_<0>, C>;

export template<typename C = identity_>
using i0_ = index_<sizet_<0>, C>;
export template<typename C = identity_>
using i1_ = index_<sizet_<1>, C>;
export template<typename C = identity_>
using i2_ = index_<sizet_<2>, C>;
export template<typename C = identity_>
using i3_ = index_<sizet_<3>, C>;
export template<typename C = identity_>
using i4_ = index_<sizet_<4>, C>;
export template<typename C = identity_>
using i5_ = index_<sizet_<5>, C>;
export template<typename C = identity_>
using i6_ = index_<sizet_<6>, C>;
export template<typename C = identity_>
using i7_ = index_<sizet_<7>, C>;
export template<typename C = identity_>
using ui0_ = unpack_<index_<sizet_<0>, C>>;
export template<typename C = identity_>
using ui1_ = unpack_<index_<sizet_<1>, C>>;
export template<typename C = identity_>
using ui2_ = unpack_<index_<sizet_<2>, C>>;
export template<typename C = identity_>
using ui3_ = unpack_<index_<sizet_<3>, C>>;
export template<typename C = identity_>
using ui4_ = unpack_<index_<sizet_<4>, C>>;
export template<typename C = identity_>
using ui5_ = unpack_<index_<sizet_<5>, C>>;
export template<typename C = identity_>
using ui6_ = unpack_<index_<sizet_<6>, C>>;
export template<typename C = identity_>
using ui7_ = unpack_<index_<sizet_<7>, C>>;

template <std::size_t N, typename I, typename C>
struct dispatch<N, index_<I, C>> : dispatch<N, drop_<I, front_<C>>> {};

template <std::size_t N, typename C>
struct dispatch<N, index_<nothing_, C>> {
    template <typename... Ts>
    using f = nothing_;
};
template <std::size_t N, typename C>
struct dispatch<N, index_<sizet_<0>, C>> {
    template <typename T0, typename... Ts>
    using f = dispatch<1, C>::template f<T0>;
};
template <std::size_t N, typename C>
struct dispatch<N, index_<sizet_<1>, C>> {
    template <typename T0, typename T1, typename... Ts>
    using f = dispatch<1, C>::template f<T1>;
};
template <std::size_t N, typename C>
struct dispatch<N, index_<sizet_<2>, C>> {
    template <typename T0, typename T1, typename T2, typename... Ts>
    using f = dispatch<1, C>::template f<T2>;
};
template <std::size_t N, typename C>
struct dispatch<N, index_<sizet_<3>, C>> {
    template <typename T0, typename T1, typename T2, typename T3, typename... Ts>
    using f = dispatch<1, C>::template f<T3>;
};
template <std::size_t N, typename C>
struct dispatch<N, index_<sizet_<4>, C>> {
    template<typename T0, typename T1, typename T2, typename T3,
                typename T4, typename... Ts>
    using f = dispatch<1, C>::template f<T4>;
};
template <std::size_t N, typename C>
struct dispatch<N, index_<sizet_<5>, C>> {
    template<typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename... Ts>
    using f = dispatch<1, C>::template f<T5>;
};
template <std::size_t N, typename C>
struct dispatch<N, index_<sizet_<6>, C>> {
    template<typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename... Ts>
    using f = dispatch<1, C>::template f<T6>;
};
template <std::size_t N, typename C>
struct dispatch<N, index_<sizet_<7>, C>> {
    template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7, typename... Ts>
    using f = dispatch<1, C>::template f<T7>;
};

namespace index_test {

template<typename T> requires(std::same_as<T, int_<3>>)
struct ElementAtIndexTwoIsThree;

template<typename T> requires(std::same_as<T, int_<1>>)
struct UnpackedElementAtIndexZeroIsOne;

template<typename T> requires(std::same_as<T, int_<1>>)
struct ElementAtFrontIsOne;

template<typename T> requires(std::same_as<T, int_<1>>)
struct EighthElement;

// Get index 2 of pack (int_<3>)
using test_one = ElementAtIndexTwoIsThree<call_<index_<uint_<2>>, int_<1>, char_<'c'>, int_<3>, int_<4>>>;

// Unpack simple_list, then take index 0
using test_two = UnpackedElementAtIndexZeroIsOne<call_<ui0_<>, list_<int_<1>, int_<2>, int_<3>>>>;

// Get first element of pack
using test_three = ElementAtFrontIsOne<call_<front_<>, int_<1>, int_<2>, int_<3>>>;

} // namespace index_test

} // namespace boost::tmp

namespace boost::tmp { // take_ / take_last_

// take_ :
export template <typename N = sizet_<0>, typename C = listify_>
struct take_ {};

// take_ : implementation
template <std::size_t N, typename P, typename C>
struct dispatch<N, take_<P, C>> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)),
                    rotate_<P, drop_<sizet_<(sizeof...(Ts) - P::value)>, C>>>::template f<Ts...>;
};

// take_last_ :
export template<typename N = sizet_<0>, typename C = listify_>
struct take_last_ {};

// take_last_ : implementation
template<std::size_t N, typename P, typename C>
struct dispatch<N, take_last_<P, C>> {
    template<typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)), drop_<sizet_<(sizeof...(Ts) - P::value)>, C>>::template f<Ts...>;
};

namespace take_test {

template<typename T> requires(std::same_as<T, list_<int_<1>>>)
struct TakeFirstElement_One;

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>>>)
struct TakeFirstTwoElements_OneTwo;

using test_one = TakeFirstElement_One<call_<take_<uint_<1>>, int_<1>, int_<2>, int_<3>>>;
using test_two = TakeFirstTwoElements_OneTwo<call_<take_<uint_<2>>, int_<1>, int_<2>, int_<3>>>;

// UNDER CONSIDERATION: Taking more than the list results in compilation failure.
// UNDER CONSIDERATION: Taking from an empty input results in compilation failure.
// list_<>{} = call_<take_<uint_<4>>>{};

template<typename T> requires(std::same_as<T, list_<int_<4>, int_<5>>>)
struct LastTwoElements;

using take_last_test_1 = LastTwoElements<call_<take_last_<int_<2>>, int_<1>, int_<2>, int_<3>, int_<4>, int_<5>>>;

} // namespace take_test

} // namespace boost::tmp

namespace boost::tmp { // transform_

// transform_ : 
export template <typename F = identity_, typename C = listify_>
struct transform_ {};

// transform_ : implementation
template <std::size_t N, typename F, typename C>
struct dispatch<N, transform_<F, C>> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)), C>::template f<
                    typename dispatch<1, F>::template f<Ts>...>;
};
template <std::size_t N, template <typename...> class F, typename FC, typename C>
struct dispatch<N, transform_<lift_<F, FC>, C>> {
    template <typename... Ts>
    using f = dispatch<(N + (N > sizeof...(Ts))), C>::template f<F<Ts>...>;
};

namespace transform_test {

} // namespace transform_test

} // namespace boost::tmp

namespace boost::tmp { // swap_

// swap_ : Swaps two variadic parametic pack values. Must be only two values.
export template <typename C = listify_>
struct swap_ {};

// swap_ : implementation
template <typename C>
struct dispatch<2, swap_<C>> {
    template <typename T, typename U>
    using f = dispatch<2, C>::template f<U, T>;
};

namespace swap_test {

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<0>>>)
struct ListOneZero;

using test_one = ListOneZero<call_<swap_<>, int_<0>, int_<1>>>;

} // namespace swap_test

} // namespace boost::tmp

namespace boost::tmp { // window_

// window_ :
export template<typename StartIndex, typename Count, typename C = listify_>
struct window_ {};

// window_ : implemenation
template<std::size_t N, typename StartIndex, typename Count, typename C>
struct dispatch<N, window_<StartIndex, Count, C>> {
    template<typename... Ts>
    using f = dispatch<sizeof...(Ts), drop_<StartIndex, take_<Count, C>>>::template f<Ts...>;
};

namespace window_test {

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>, int_<3>>>)
struct GetTheMiddle123;

using window_test_1 = GetTheMiddle123<call_<window_<int_<1>, int_<3>>, int_<0>, int_<1>, int_<2>, int_<3>, int_<5>, int_<7>>>;

} // namespace window_test

} // namespace boost::tmp

namespace boost::tmp { // keys_ / values_ / nth_values_

// keys_ :
// Input params: Parameter pack
// Closure params: C - Continuation; default listify_
// Functional description:
// input  - T0<t00, t01, ...>, T1<t10, t11, ...>, ..., TN<tN0, tN1, ...>
// apply  - (get 1st element from each element)
// result - list_<t00, t10, ..., tN0>
// Empty return type: list_<>
export template<typename C = listify_>
struct keys_ {};

// keys_ : implementation
template <std::size_t N, typename C>
struct dispatch<N, keys_<C>> : dispatch<N, transform_<ui0_<>, C>> {};

// values_ :
// Input params: Parameter pack
// Closure params: C - Continuation; default listify_
// Functional description:
// input  -  T0<t00, t01, ...>, T1<t10, t11, ...>, ..., TN<tN0, tN1, ...>
// apply  -  (get 2nd element from each element)
// result -  list_<t01, t11, ..., tN1>
// Empty return type: list_<>
export template<typename C = listify_>
struct values_ {};

// values_ : implementation
template <std::size_t N, typename C>
struct dispatch<N, values_<C>> : dispatch<N, transform_<ui1_<>, C>> {};

// nth_values_ :
// Input params: Parameter pack
// Closure params: I - Positive integer type of the index to take from each element in pack.
//                 C - Continuation; default listify_
// Functional description:
// input  -  T0<t00, t01, ..., T0I, ...>, T1<t10, t11, ..., T1I, ...>, ..., TN<tN0, tN1, ..., TNI, ...>
// apply  -  (get Ith element from each element)
// result -  list_<T0I, T1I, ..., TNI>
// Empty return type: list_<>
export template<typename I, typename C = listify_>
struct nth_values_ {};

// nth_values_ : implementation
template <std::size_t N, typename I, typename C>
struct dispatch<N, nth_values_<I, C>> : dispatch<N, transform_<unpack_index_<I>, C>> {};

namespace nth_values_tests {

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>>>)
struct KeysOfMap;

template<typename T> requires(std::same_as<T, list_<int_<44>, int_<45>>>)
struct ValuesOfMap;

template<typename T> requires(std::same_as<T, list_<int_<22>, int_<23>>>)
struct IndexTwoValuesList;

using keys_test_1 = KeysOfMap<call_<keys_<>, list_<int_<1>, int_<22>>, list_<int_<2>, int_<10>>>>;

using values_test_1 = ValuesOfMap<call_<values_<>, list_<int_<1>, int_<44>>, list_<int_<2>, int_<45>>>>;

using nth_values_test_1 = IndexTwoValuesList<call_<nth_values_<sizet_<2>>,
                                                   list_<int_<1>, int_<22>, int_<22>>,
                                                   list_<int_<2>, int_<10>, int_<23>>>>;

} // namespace nth_values_tests

} // namespace boost::tmp

namespace boost::tmp { // push_back_ / pop_back_ / push_front_ / pop_front_

// push_back_ :
export template <typename T, typename C = listify_>
struct push_back_ {};

// push_back_ : implementation
template <std::size_t N, typename T, typename C>
struct dispatch<N, push_back_<T, C>> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1), C>::template f<Ts..., T>;
};

// pop_front_ :
export template <typename C = listify_>
struct pop_front_ {};

// pop_front_ : implementation
template <std::size_t N, typename C>
struct dispatch<N, pop_front_<C>> {
    template <typename T, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)), C>::template f<Ts...>;
};
// TODO: Should this be kept? This differs from the behavior of popping
// the front off of an empty list (which results in an empty list).
// This, however, will return a list_<nothing_>
template <typename C>
struct dispatch<0, pop_front_<C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<nothing_>;
};

// push_front_ :
export template <typename T, typename C = listify_>
struct push_front_ {};

// push_front_ : implementation
template <std::size_t N, typename T, typename C>
struct dispatch<N, push_front_<T, C>> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts) + 1), C>::template f<T, Ts...>;
};

// pop_back_ :
export template <typename C = listify_>
struct pop_back_ {};
// // pop_back_ : implementation
template<std::size_t N, typename C>
struct dispatch<N, pop_back_<C>> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)), rotate_<sizet_<sizeof...(Ts) - 1>, pop_front_<
                    rotate_<sizet_<(sizeof...(Ts) - 1)>, C>>>>::template f<Ts...>;
};
template <typename C>
struct dispatch<0, pop_back_<C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<nothing_>;
};

namespace push_pop_tests {

// push_back_ tests
template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>, char_<'c'>>>)
struct PushBack_C {};

using push_back_test_1 = PushBack_C<call_<push_back_<char_<'c'>>, int_<1>, int_<2>>>;

// pop_front_ tests
template<typename T> requires(std::same_as<T, list_<>>)
struct NoElementsLeft;

template<typename T> requires(std::same_as<T, list_<nothing_>>)
struct EmptyPackReturnsListWithNothingType;

// Pop front off of single element list to return an empty list.
using pop_front_test_1 = NoElementsLeft<call_<pop_front_<>, list_<int_<1>>>>;

// Pop front off of empty list_ to return an empty list_
using pop_front_test_2 = NoElementsLeft<call_<pop_front_<>, list_<>>>;

// UNDER CONSIDERATION: Removal / modification of behavior of
// pop_front_ on no input. Currently returns a list_<nothing_>
using pop_front_test_3 = EmptyPackReturnsListWithNothingType<call_<pop_front_<>>>;

// push_front tests
template<typename T> requires(std::same_as<T, list_<int_<4>, int_<1>, int_<2>>>)
struct PushFourToFront;

template<typename T> requires(std::same_as<T, list_<int_<1>>>)
struct PushOneToEmptyPack;

using push_front_test_1 = PushFourToFront<call_<push_front_<int_<4>>, int_<1>, int_<2>>>;

using push_front_test_2 = PushOneToEmptyPack<call_<push_front_<int_<1>>>>;

// pop_back_ tests
template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>>>)
struct Pop_int_3_OffOfBack;

template<typename T> requires(std::same_as<T, list_<>>)
struct PopBackEmptyList;

template<typename T> requires(std::same_as<T, list_<nothing_>>)
struct PopBackEmptyPack;

using pop_back_test_1 = Pop_int_3_OffOfBack<call_<pop_back_<>, int_<1>, int_<2>, int_<3>>>;

using pop_back_test_2 = PopBackEmptyList<call_<pop_back_<>, list_<>>>;

using pop_back_test_3 = PopBackEmptyPack<call_<pop_back_<>>>;

} // namespace push_pop_tests

} // namespace boost::tmp

namespace boost::tmp { // slice_

template<typename LowerB, typename UpperB, typename C = listify_>
struct slice_ {};

template<std::size_t N, typename LowerB, typename UpperB, typename C>
struct dispatch<N, slice_<LowerB, UpperB, C>> : dispatch<N, drop_<LowerB, drop_last_<UpperB, C>>> {};

namespace slice_test {

template<typename T> requires(std::same_as<T, list_<int_<5>, int_<6>, int_<7>, int_<8>>>)
struct LBFourUBTwoSlice;

using slice_test_1 = LBFourUBTwoSlice<call_<slice_<sizet_<4>, sizet_<2>>,
                                            int_<1>, int_<2>, int_<3>, int_<4>, int_<5>,
                                            int_<6>, int_<7>, int_<8>, int_<9>, int_<10>>>;

} // namespace slice_test

} // namespace boost::tmp

namespace boost::tmp { // each_

// each_ :
export template <typename... Fs>
struct each_ {};

// each_ : implementation
template <typename F, typename C>
struct dispatch<1, each_<F, C>> {
    template <typename T>
    using f = dispatch<1, C>::template f<dispatch<1, F>::template f<T>>;
};
template <typename F0, typename F1, typename C>
struct dispatch<2, each_<F0, F1, C>> {
    template <typename T0, typename T1>
    using f = dispatch<2, C>::template f<dispatch<1, F0>::template f<T0>,
                                            dispatch<1, F1>::template f<T1>>;
};
template <typename F0, typename F1, typename F2, typename C>
struct dispatch<3, each_<F0, F1, F2, C>> {
    template <typename T0, typename T1, typename T2>
    using f = dispatch<3, C>::template f<dispatch<1, F0>::template f<T0>,
                                            dispatch<1, F1>::template f<T1>,
                                            dispatch<1, F2>::template f<T2>>;
};
template <typename F0, typename F1, typename F2, typename F3, typename C>
struct dispatch<4, each_<F0, F1, F2, F3, C>> {
    template <typename T0, typename T1, typename T2, typename T3>
    using f = dispatch<4, C>::template f<dispatch<1, F0>::template f<T0>,
                                            dispatch<1, F1>::template f<T1>,
                                            dispatch<1, F2>::template f<T2>,
                                            dispatch<1, F3>::template f<T3>>;
};

namespace each_test {

} // namespace each_test

} // namespace boost::tmp

namespace boost::tmp { // erase_

// erase_ : Given a VPP, remove the nth value in the pack.
// Reduces the size of the list by 1.
// Input params: Parameter pack
// Closure params: N - Positive integer type of index of element to remove
//                 C - Continuation; default listify_
// Functional description:
// input  -  T0, T1, ..., T(M - 1), TM, T(M + 1), ..., TN
// apply  -  (erase element at index M)
// result -  T0, T1, ..., T(M - 1), T(M + 1), ..., TN
// Empty return type: list_<>
export template <typename N = sizet_<0>, typename C = listify_>
struct erase_ {};

// erase_ : implementation
template <std::size_t N, typename I, typename C>
struct dispatch<N, erase_<I, C>> {
    template <typename... Ts>
    using f = dispatch<N,
        rotate_<I,
            pop_front_<rotate_<sizet_<(sizeof...(Ts) - I::value - 1)>, C>>>>::template f<Ts...>;
};
template <typename I, typename C>
struct dispatch<0, erase_<I, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<nothing_>;
};

namespace erase_test {

template<typename T> requires(std::same_as<T, list_<int_<2>, int_<3>>>)
struct EraseFirstElement;

// Erasing an element from a list with a single element will always result in empty list.
template<typename T> requires(std::same_as<T, list_<>>)
struct EraseSingleElement;

// UNDER RECONSIDERATION: Returns list_<nothing_> if no input is given.
template<typename T> requires(std::same_as<T, list_<nothing_>>)
struct EmptyPackReturnsNothingType;

using test_one   = EraseFirstElement<call_<erase_<sizet_<0>>, int_<1>, int_<2>, int_<3>>>;

using test_two   = EraseSingleElement<call_<erase_<sizet_<0>>, int_<0>>>;

using test_three = EmptyPackReturnsNothingType<call_<erase_<sizet_<0>>>>;

} // namespace erase_test

} // namespace boost::tmp

namespace boost::tmp { // insert_

// @BUG: This does not work as expected.
// Inserting will cause a permutation of the current list with the
// to-be-inserted value. Also, as long as N < size of input list, it'll compile.
// Attempting to insert a value in a position greater than the size of the list
// will result in an error, as the value goes negative.
export template <typename N, typename V, typename C = listify_>
struct insert_ {};

// insert_ : implementation
template <std::size_t N, typename I, typename V, typename C>
struct dispatch<N, insert_<I, V, C>> {
    template <typename... Ts>
    using f = dispatch<N, rotate_<I, push_front_<V, rotate_<sizet_<(sizeof...(Ts) - I::value + 1)>,
                                                    C>>>>::template f<Ts...>;
};
template <typename I, typename V, typename C>
struct dispatch<0, insert_<I, V, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<V>;
};

namespace insert_test {

// Insert char_<'c'> at position 1
template<typename T> requires(std::same_as<T, list_<int_<1>, char_<'c'>, int_<2>>>)
struct Insert_C_AtPositionOne;

// Insert char_<'c'> at position 0
template<typename T> requires(std::same_as<T, list_<char_<'c'>, int_<1>, int_<2>>>)
struct Insert_C_AtPositionZero;

// Insert char_<'c'> at position 2
template<typename T> requires(std::same_as<T, list_<int_<1>, int_<2>, char_<'c'>>>)
struct Insert_C_AtPositionTwo;

// Insert char_<'c'> into no list (returns a list_ with char_<'c'> in it)
template<typename T> requires(std::same_as<T, list_<char_<'c'>>>)
struct EmptyPackInsertionReturnsSingleElementList;

using test_one   = Insert_C_AtPositionZero<call_<insert_<int_<0>, char_<'c'>>, int_<1>, int_<2>>>;
using test_two   = Insert_C_AtPositionOne<call_<insert_<int_<1>, char_<'c'>>, int_<1>, int_<2>>>;
using test_three = Insert_C_AtPositionTwo<call_<insert_<int_<2>, char_<'c'>>, int_<1>, int_<2>>>;
using test_four  = EmptyPackInsertionReturnsSingleElementList<call_<insert_<int_<0>, char_<'c'>>>>;

} // namespace insert_test

} // namespace boost::tmp

namespace boost::tmp { // make_sequence_ / make_seq_impl / repeat_sequence_

// FIXME: make_seq_impl used elsewhere.
// TODO: Generalized to all integers.

// make_sequence_ :
export template <typename F = identity_, typename C = listify_>
struct make_sequence_ {};

// make_sequence_ : implementation
consteval std::size_t next_number(std::size_t current, std::size_t end) {
    return ((end - 2 * current) < 2) ?
                    end :
                    next_number(current,
                                end / 2); // note that std::size_t / 2 always rounds down
}
consteval std::size_t next_state(std::size_t current, std::size_t end) {
    return ((end - current) < 2) ? end - current :
                                    (2 + (next_number(current, end) - 2 * current));
}
template <std::size_t State>
struct make_seq_impl;
template <>
struct make_seq_impl<0> { // done
    template <std::size_t End, std::size_t... Is>
    using f = list_<sizet_<Is>...>;
};
template <>
struct make_seq_impl<1> { // done +1
    template <std::size_t End, std::size_t... Is>
    using f = list_<sizet_<Is>..., sizet_<End - 1>>;
};
template <>
struct make_seq_impl<2> { // double
    template <std::size_t End, std::size_t... Is>
    using f = make_seq_impl<next_state(2 * sizeof...(Is), End)>::template 
                f<End, Is..., (Is + sizeof...(Is))...>;
};
template <>
struct make_seq_impl<3> { // double +1
    template <std::size_t End, std::size_t... Is>
    using f = make_seq_impl<next_state(2 * sizeof...(Is) + 1, End)>::template f<
                    End, Is..., (Is + sizeof...(Is))..., (2 * sizeof...(Is))>;
};
template <typename F, typename C>
struct dispatch<1, make_sequence_<F, C>> {
    template <typename N>
    using f = dispatch<1, unpack_<transform_<F, C>>>::template f<
            typename make_seq_impl<next_state(0, N::value)>::template f<N::value>>;
};

// TODO: Benchmark alternate implemenation.
// template<typename... Vs>
// struct index_impl;
// template<std::size_t... Vs>
// struct index_impl<std::index_sequence<Vs...>> {
//     using f = list_<sizet_<Vs>...>;
// };
// template <typename F, typename C>
// struct dispatch<1, make_sequence_<F, C>> {
//     template <typename N>
//     using f = dispatch<1, unpack_<transform_<F, C>>>::template f<typename index_impl<std::make_index_sequence<N::value>>::template f>;
// };

// repeat_sequence_ :
export template<typename N = sizet_<0>, typename C = listify_>
struct repeat_sequence_{};

// repeat_sequence_ : implementation
template <std::size_t, typename C>
struct repeat_seq_impl;
template <typename C>
struct repeat_seq_impl<0, C> {
    template <typename...>
    using f = dispatch<0, C>::template f<>;
};
template <typename C>
struct repeat_seq_impl<1, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 1, C>::template f<T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<2, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 2, C>::template f<T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<3, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 3, C>::template f<T, T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<4, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 4, C>::template f<T, T, T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<5, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 5, C>::template f<T, T, T, T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<6, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 6, C>::template f<T, T, T, T, T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<7, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 7, C>::template f<T, T, T, T, T, T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<8, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 8, C>::template f<T, T, T, T, T, T, T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<16, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 16, C>::template f<T, T, T, T, T, T, T, T,
                                                          T, T, T, T, T, T, T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<32, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 32, C>::template f<T, T, T, T, T, T, T, T,
                                                          T, T, T, T, T, T, T, T,
                                                          T, T, T, T, T, T, T, T,
                                                          T, T, T, T, T, T, T, T, Ts...>;
};
template <typename C>
struct repeat_seq_impl<64, C> {
    template <typename T, typename... Ts>
    using f = dispatch<sizeof...(Ts) + 64, C>::template f<
            T, T, T, T, T, T, T, T,
            T, T, T, T, T, T, T, T,
            T, T, T, T, T, T, T, T,
            T, T, T, T, T, T, T, T,
            T, T, T, T, T, T, T, T,
            T, T, T, T, T, T, T, T,
            T, T, T, T, T, T, T, T,
            T, T, T, T, T, T, T, T, Ts...>;
};
template <std::size_t P, typename C, std::size_t Step = step_selector(P)>
struct make_repeat : repeat_seq_impl<Step, repeat_sequence_<sizet_<(P - Step) + 1>, C>> {};
template <std::size_t P, typename C>
struct make_repeat<P, C, P> : repeat_seq_impl<P, C> {};
template <std::size_t N, typename P, typename C>
struct dispatch<N, repeat_sequence_<P, C>> : make_repeat<P::value, C> {};

// TODO: Merge these together.
namespace make_sequence_test {

template<typename T>
using and_one = sizet_<T::value + 1>;

template<typename T>
struct always_one {
    template<typename N>
    using f = sizet_<T::value>;
};

template<typename T> requires(std::same_as<T, list_<sizet_<0>, sizet_<1>, sizet_<2>>>)
struct ListZeroOneTwo;

template<typename T> requires(std::same_as<T, list_<sizet_<1>, sizet_<2>, sizet_<3>>>)
struct ListOneTwoThree;

using test_one = ListZeroOneTwo<call_<make_sequence_<>, sizet_<3>>>;
using test_two = ListOneTwoThree<call_<make_sequence_<lift_<and_one>>, sizet_<3>>>;
} // namespace make_sequence_test

namespace repeat_sequence_test {

// TODO: Implement repeat_sequence_test
// std::index_sequence<1, 1, 1, 1, 1, 1, 1, 1, 1, 1>{} = call_<repeat_sequence_<sizet_<10>, lift_<into_sequence>>, sizet_<1>>{};
// list_<sizet_<1>, sizet_<1>, sizet_<1>, sizet_<2>, sizet_<2>, sizet_<2>, sizet_<3>, sizet_<3>, sizet_<3>>{} = call_<transform_<repeat_sequence_<sizet_<3>>, join_<>>, sizet_<1>, sizet_<2>, sizet_<3>>{};
} // namespace repeate_sequence_test

} // namespace boost::tmp

namespace boost::tmp { // zip_ / zip_with_index_

// FIXME: zip_ should have a dispatching functionality similar to
// rotate or other large dispatching functions.

// zip_ :
// Input params: Parameter pack
// Closure params: F - Metaclosure that accepts input parameter types in parameter pack.
//                 C - Continuation; default listify_
// Functional description:
// input  - T0, T1, ..., TN
// apply  - (invoke F on each element)
// result - list_<F<T0>, F<T1>, ..., F<TN>>
// Empty / default return type: list_<>
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

// zip_with_index_ : Enumerates over a parameter pack, indexing each
// element starting at 0.
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

// TODO: Merge tests
namespace zip_test {

template<typename T> requires(std::same_as<T, list_<int_<3>, int_<7>>>)
struct AddPairsTogetherWithZip;

template<typename T, typename U>
using add = int_<T::value + U::value>;

template<typename T> requires(std::same_as<T, list_<list_<int_<1>, int_<2>>, list_<int_<3>, int_<4>>>>)
struct ZipPairsIntoList;

// Performs an addition of pairs of elements component wise i.e. (x0 + x1), (y0 + y1), ...
using zip_test_1 = AddPairsTogetherWithZip<call_<zip_<lift_<add>>, list_<int_<1>, int_<3>>, list_<int_<2>, int_<4>>>>;

// Pairs elements in each list together i.e. (x0, y0), (x1, y1), ...
using zip_test_2 = ZipPairsIntoList<call_<zip_<listify_>, list_<int_<1>, int_<3>>, list_<int_<2>, int_<4>>>>;

} // namespace zip_test

namespace zip_with_index_test {

template<typename T> requires(std::same_as<T, list_<list_<sizet_<0>, char_<'a'>>,
                                                    list_<sizet_<1>, char_<'b'>>,
                                                    list_<sizet_<2>, char_<'c'>>>>)
struct ZipABCWith123;
using test_one = ZipABCWith123<call_<zip_with_index_<>, char_<'a'>, char_<'b'>, char_<'c'>>>;
} // namespace zip_with_index_test

} // namespace boost::tmp

namespace boost::tmp { // foldey, not exported

consteval std::size_t select_foldey_loop(std::size_t rest_size) {
    return static_cast<std::size_t>(rest_size < 8 ? (rest_size == 0 ? 1000 : 1001) : 1008);
}

// FIXME: Workaround employed after value isn't found when invoking F::template f<T0>::value
// in foldey<1001>. The value of the type is found after referring to it here, not in foldey<1001>.
template<typename WorkAroundT>
consteval std::size_t select_foldey(std::size_t chunk_size, std::size_t rest_size) {
    return WorkAroundT::value == std::numeric_limits<std::size_t>::max() ? select_foldey_loop(rest_size) :
                                    chunk_size - WorkAroundT::value;
}

template <std::size_t S>
struct foldey {
    template <typename F, std::size_t N, typename...>
    using f = sizet_<N - S>;
};
template <>
struct foldey<1000> {
    template <typename F, std::size_t N, typename... Ts>
    using f = nothing_;
};
// FIXME: Workaround for extracting value in F::template f<T0>. Pass type into now templated select_foldey.
template <>
struct foldey<1001> {
    template <typename F, std::size_t N, typename T0, typename... Ts> 
    using f = foldey<select_foldey<typename F::template f<T0>>(1, sizeof...(Ts))>::template f<F, N + 1, Ts...>;
};
template <>
struct foldey<1008> {
    template <typename F, std::size_t N, typename T0, typename T1, typename T2,
                typename T3, typename T4, typename T5, typename T6, typename T7,
                typename... Ts>
    using f = foldey<select_foldey(8, sizeof...(Ts),
                F::template
                  f<T0>::template
                    f<T1>::template
                      f<T2>::template
                        f<T3>::template
                          f<T4>::template
                            f<T5>::template
                              f<T6>::template
                                f<T7>::value)>::template
                                  f<F, N + 8, Ts...>;
};
template <>
struct foldey<1000000> {};

} // export namespace boost::tmp

namespace boost::tmp { // logic metaclosures

// is_ : 
export template <typename P, typename C = identity_>
struct is_ {};

// is_ implementation
template <typename P, typename C>
struct dispatch<1, is_<P, C>> {
    template <typename T>
    using f = dispatch<1, C>::template f<bool_<std::is_same_v<P, T>>>;
};

// not_ : 
export template <typename C = identity_>
struct not_ {};

// not_ : implementation
template <typename C>
struct dispatch<1, not_<C>> {
    template <typename T>
    using f = dispatch<1, C>::template f<bool_<(!T::value)>>;
};

// or_ : 
export template <typename F = identity_, typename C = identity_>
struct or_ {};

// or_ : implementation
template <bool Short, template <typename...> class F>
struct ory {
    template <typename T>
    using f                    = ory<F<T>::value, F>;
    static constexpr std::size_t value = -1;
};
template <template <typename...> class F>
struct ory<true, F> {
    template <typename T>
    using f                    = ory;
    static constexpr std::size_t value = 1;
};
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, or_<lift_<F>, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<
            call_<is_<nothing_, not_<identity_>>,
                    typename foldey<(select_foldey_loop(
                            sizeof...(Ts)))>::template f<ory<false, F>, 0, Ts...>>>;
};
template <template <typename...> class F, typename C>
struct dispatch<0, or_<lift_<F>, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<false_>;
};
template <std::size_t N, typename F, typename C>
struct dispatch<N, or_<F, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<
            call_<is_<nothing_, not_<identity_>>,
                    typename foldey<(select_foldey_loop(sizeof...(Ts)))>::template f<
                            ory<false, dispatch<1, F>::template f>, 0, Ts...>>>;
};
template <typename F, typename C>
struct dispatch<0, or_<F, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<false_>;
};

// and_ : 
export template <typename F = identity_, typename C = identity_>
struct and_ {};

// and_ : implementation
template <bool Short, template <typename...> class F>
struct andy {
    template <typename T>
    using f                    = andy<(!F<T>::value), F>;
    static constexpr std::size_t value = -1;
};
template <template <typename...> class F>
struct andy<true, F> {
    template <typename T>
    using f                    = andy;
    static constexpr std::size_t value = 1;
};
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, and_<lift_<F>, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<
            call_<is_<nothing_>, typename foldey<(select_foldey_loop(sizeof...(
                                            Ts)))>::template f<andy<false, F>, 0, Ts...>>>;
};
template <template <typename...> class F, typename C>
struct dispatch<0, and_<lift_<F>, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<false_>;
};
template <std::size_t N, typename F, typename C>
struct dispatch<N, and_<F, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<
            call_<is_<nothing_>,
                    typename foldey<(select_foldey_loop(sizeof...(Ts)))>::template f<
                            andy<false, dispatch<1, F>::template f>, 0, Ts...>>>;
};
template <typename F, typename C>
struct dispatch<0, and_<F, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<false_>;
};

// if_ : Given a predicate P, if true, return T, and if false, return F.
export template <typename P, typename T, typename F = always_<nothing_>>
struct if_ {};

// if_ implementation
template <bool B>
struct if_impl;
template <>
struct if_impl<true> {
    template <typename T, typename U>
    using f = T;
};
template <>
struct if_impl<false> {
    template <typename T, typename U>
    using f = U;
};
template <typename P, typename T, typename F>
struct dispatch<1, if_<P, T, F>> {
    template <typename T0>
    using f = dispatch<1, typename if_impl<dispatch<1, P>::template
                    f<T0>::value>::template
                        f<T, F>>::template
                            f<T0>;
};
template <template <typename...> class P, typename T, typename F>
struct dispatch<1, if_<lift_<P>, T, F>> {
    template <typename T0>
    using f = dispatch<1, typename if_impl<P<T0>::value>::template
                    f<T, F>>::template
                        f<T0>;
};
template <std::size_t N, typename P, typename T, typename F>
struct dispatch<N, if_<P, T, F>> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)),
                    typename if_impl<dispatch<find_dispatch(sizeof...(Ts)), P>::template
                        f<Ts...>::value>::template f<T, F>>::template f<Ts...>;
};
template <std::size_t N, template <typename...> class P, typename T, typename F>
struct dispatch<N, if_<lift_<P>, T, F>> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)),
                    typename if_impl<P<Ts...>::value>::template
                        f<T, F>>::template f<Ts...>;
};
template <template <typename...> class P, typename T, typename F>
struct dispatch<2, if_<lift_<P>, T, F>> {
    template <typename T0, typename T1>
    using f = dispatch<2, typename if_impl<P<T0,T1>::value>::template
                    f<T, F>>::template f<T0, T1>;
};
template <template <typename...> class P>
struct dispatch<1, if_<lift_<P>, listify_, always_<list_<>>>> {
    template <typename U>
    using f = if_impl<P<U>::value>::template f<list_<U>, list_<>>;
};

// less_ : 
export template <typename V = nothing_, typename C = identity_>
struct less_ {};

// less_ : implementation
template <typename C>
struct dispatch<2, less_<C>> {
    template <typename T, typename U>
    using f = dispatch<1, C>::template f<bool_<(T::value)<(U::value)>>;
};
template <typename U, typename C>
struct dispatch<1, less_<U, C>> {
    template<typename T>
    using f = dispatch<1, C>::template f<bool_<(U::value)<(T::value)>>;
};

// less_f_ : 
export template <typename F, typename C = identity_>
struct less_f_ {};

// less_f_ : implementation
template <template<typename...> typename F, typename C>
struct dispatch<2, less_f_<lift_<F>, C>> {
    template<typename T, typename U>
    using f = dispatch<1, C>::template f<bool_<(F<T>::value < F<U>::value)>>;
};

// less_eq_ : 
export template <typename V = nothing_, typename C = identity_>
struct less_eq_ {};

// less_eq_ : implementation
template <typename C>
struct dispatch<2, less_eq_<C>> {
    template <typename T, typename U>
    using f = dispatch<1, C>::template f<bool_<(T::value)<=(U::value)>>;
};
template <typename U, typename C>
struct dispatch<1, less_eq_<U, C>> {
    template<typename T>
    using f = dispatch<1, C>::template f<bool_<(U::value)<=(T::value)>>;
};

// greater_ : 
export template <typename V = nothing_, typename C = identity_>
struct greater_ {};

// greater_ : implementation
template <typename C>
struct dispatch<2, greater_<C>> {
    template <typename T, typename U>
    using f = dispatch<1, C>::template f<bool_<(U::value)<(T::value)>>;
};
template <typename U, typename C>
struct dispatch<1, greater_<U, C>> {
    template<typename T>
    using f = dispatch<1, C>::template f<bool_<(T::value)<(U::value)>>;
};

// range_lo_hi_ : 
export template <typename LV = nothing_, typename UV = nothing_, typename C = identity_>
struct range_lo_hi_ {};

// range_lo_hi_ : implementation
template <typename Lower, typename Upper, typename C>
struct dispatch<1, range_lo_hi_<Lower, Upper, C>> {
    template<typename T>
    using f = dispatch<1, C>::template f<bool_<!((Lower::value < T::value) && (T::value < Upper::value))>>;
};

// all_of_ : Given a unary predicate, return true_ / false_ on whether all elements
// in a VPP satisfy that predicate.
export template <typename F, typename C = identity_>
struct all_of_ {};

// all_of_ : implementation
template <std::size_t N, typename F, typename C>
struct dispatch<N, all_of_<F, C>> : dispatch<N, and_<F, C>> {};

// any_of_ : Given a unary predicate, return true_ / false_ on whether any elements
// in a VPP satisfy that predicate.
export template <typename F = identity_, typename C = identity_>
struct any_of_ {};

// any_of_ : implementation
template <std::size_t N, typename F, typename C>
struct dispatch<N, any_of_<F, C>> : dispatch<N, or_<F, C>> {};

// none_of_ : Given a predicate (F), return true_ / false_ if none
// of the elements in a VPP satisfy the predicate F.
export template <typename F, typename C = identity_>
struct none_of_ {};

// none_of_ : implementation
template <std::size_t N, typename F, typename C>
struct dispatch<N, none_of_<F, C>> : dispatch<N, and_<F, not_<C>>> {};

// contains_ : Given a type (V), return true_ / false_ on whether a given VPP
// contains the type V.
export template <typename T, typename C = identity_>
struct contains_ {};

// contains_ : implementation
template <std::size_t N, typename T, typename C>
struct dispatch<N, contains_<T, C>> : dispatch<N, or_<is_<T>, C>> {};

namespace logic_test {

template<typename T> requires(std::same_as<T, false_>)
struct AllNumbersOdd;

template<typename T> requires(std::same_as<T, true_>)
struct AllNumbersEven;

template<typename T> requires(std::same_as<T, true_>)
struct OneNumberEven;

template<typename T> requires(std::same_as<T, true_>)
struct OneNumberOdd;

template<typename T>
using is_even = bool_<(T::value % 2 == 0)>;

using and_test_1 = AllNumbersOdd<call_<and_<lift_<is_even>>, int_<1>, int_<1>, int_<1>>>;

using and_test_2 = AllNumbersEven<call_<and_<lift_<is_even>>, int_<2>, int_<2>, int_<2>>>;

using or_test_1 = OneNumberEven<call_<or_<lift_<is_even>>, int_<1>, int_<2>, int_<1>>>;

using or_test_2 = OneNumberOdd<call_<or_<lift_<is_even>>, int_<2>, int_<1>, int_<2>>>;

// Contains tests
template<typename T> requires(std::same_as<T, false_>)
struct DoesNotContainType;

template<typename T> requires(std::same_as<T, true_>)
struct ContainsType;

using contains_test_1   = DoesNotContainType<call_<contains_<int_<0>>, int_<1>>>;

using contains_test_2   = ContainsType<call_<contains_<int_<2>>, int_<0>, int_<1>, int_<2>>>;

using contains_test_3   = DoesNotContainType<call_<contains_<int_<1>>>>;

// template<int A, int B>
// struct two_ints {
//     int a{A};
//     int b{B};
// };

// false_{} = call_<less_f_<lift_<std::alignment_of>>, two_ints<1, 2>, char_<'c'>>{};
// true_{}  = call_<less_f_<lift_<std::alignment_of>>, char_<'c'>,     two_ints<1, 2>>{};

template<typename T> requires(std::same_as<T, false_>)
struct NoneOfTheNumbersAreOdd;

template<typename T> requires(std::same_as<T, true_>)
struct NoneOfTheNumbersAreEven;

using none_of_test_1 = NoneOfTheNumbersAreOdd<call_<none_of_<lift_<is_even>>, int_<2>, int_<100>, int_<4>, int_<500>>>;

using none_of_test_2 = NoneOfTheNumbersAreEven<call_<none_of_<lift_<is_even>>, int_<1>, int_<3>>>;

} // namespace logic_test

} // namespace boost::tmp

namespace boost::tmp { // tee_

export template <typename... Fs>
struct tee_ {};

template <typename N, typename L, typename C, typename... Fs>
struct tee_impl {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Fs) + 1), C>::template f<
                typename dispatch<find_dispatch(sizeof...(Ts)), Fs>::template f<Ts...>...,
                typename dispatch<find_dispatch(sizeof...(Ts)), L>::template f<Ts...>>;
};
// specialization for case where last closure is a forward
template <typename N, typename C, typename... Fs>
struct tee_impl<N, identity_, C, Fs...> {
    template <typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Fs) + sizeof...(Ts)), C>::template
                f<dispatch<find_dispatch(sizeof...(Ts)), Fs>::template f<Ts...>...,
                        Ts...>;
};
template <typename L, typename C, typename... Fs>
struct tee_impl<sizet_<1>, L, C, Fs...> {
    template <typename T0>
    using f = dispatch<find_dispatch(sizeof...(Fs) + 1), C>::template f<
                typename dispatch<1, Fs>::template f<T0>...,
                typename dispatch<1, L>::template f<T0>>;
};
// specialization for case where last closure is a forward
template <typename C, typename... Fs>
struct tee_impl<sizet_<1>, identity_, C, Fs...> {
    template <typename T0>
    using f = dispatch<find_dispatch(sizeof...(Fs) + 1), C>::template f<
                typename dispatch<1, Fs>::template f<T0>..., T0>;
};
template <typename L, typename C, typename... Fs>
struct tee_impl<sizet_<2>, L, C, Fs...> {
    template <typename T0, typename T1>
    using f = dispatch<find_dispatch(sizeof...(Fs) + 1), C>::template
                    f<typename dispatch<2, Fs>::template f<T0, T1>...,
                    typename dispatch<2, L>::template f<T0, T1>>;
};
// specialization for case where last closure is a forward
template <typename C, typename... Fs>
struct tee_impl<sizet_<2>, identity_, C, Fs...> {
    template <typename T0, typename T1>
    using f = dispatch<find_dispatch(sizeof...(Fs) + 2), C>::template f<
                typename dispatch<2, Fs>::template f<T0, T1>..., T0, T1>;
};

// in case the continuation is an and_

template <bool b, typename C, typename... Fs>
struct tee_and_impl;
template <typename C, typename... Fs>
struct tee_and_impl<false, C, Fs...> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<false_>;
};
template <typename C, typename... Fs>
struct tee_and_impl<true, C, Fs...> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<true_>;
};
template <typename C, typename F, typename... Fs>
struct tee_and_impl<true, C, F, Fs...> {
    template <typename... Ts>
    using f = tee_and_impl<dispatch<find_dispatch(sizeof...(Ts)), F>::template f<Ts...>::value, C,
            Fs...>::template f<Ts...>;
};
template <bool b, typename C, typename... Fs>
struct tee_and_impl_1;
template <typename C, typename... Fs>
struct tee_and_impl_1<false, C, Fs...> {
    template <typename T0>
    using f = dispatch<1, C>::template f<false_>;
};
template <typename C, typename... Fs>
struct tee_and_impl_1<true, C, Fs...> {
    template <typename T0>
    using f1 = dispatch<1, C>::template f<true_>;
};
template <typename C, typename F, typename... Fs>
struct tee_and_impl_1<true, C, F, Fs...> {
    template <typename T0>
    using f = tee_and_impl<dispatch<1, F>::template f<T0>::value, C,
                                    Fs...>::template f<T0>;
};
template <bool b, typename C, typename... Fs>
struct tee_and_impl_2;
template <typename C, typename... Fs>
struct tee_and_impl_2<false, C, Fs...> {
    template <typename T0, typename T1>
    using f = dispatch<1, C>::template f<false_>;
};
template <typename C, typename... Fs>
struct tee_and_impl_2<true, C, Fs...> {
    template <typename T0, typename T1>
    using f = dispatch<1, C>::template f<true_>;
};
template <typename C, typename F, typename... Fs>
struct tee_and_impl_2<true, C, F, Fs...> {
    template <typename T0, typename T1>
    using f = tee_and_impl<dispatch<2, F>::template f<T0, T1>::value, C,
                                    Fs...>::template f<T0, T1>;
};

template <typename N, typename L, typename C, typename... Fs>
struct tee_impl<N, L, and_<identity_, C>, Fs...> : tee_and_impl<true, C, Fs..., L> {};
template <typename L, typename C, typename... Fs>
struct tee_impl<sizet_<1>, L, and_<identity_, C>, Fs...>
        : tee_and_impl_1<true, C, Fs..., L> {};
template <typename C, typename... Fs>
struct tee_impl<sizet_<1>, identity_, and_<identity_, C>, Fs...>
        : tee_and_impl_1<true, C, Fs..., identity_> {};
template <typename L, typename C, typename... Fs>
struct tee_impl<sizet_<2>, L, and_<identity_, C>, Fs...>
        : tee_and_impl_2<true, C, Fs..., L> {};
template <typename C, typename... Fs>
struct tee_impl<sizet_<2>, identity_, and_<identity_, C>, Fs...>
        : tee_and_impl_2<true, C, Fs..., identity_> {};

// in case the continuation is an or_

template <bool b, typename C, typename... Fs>
struct tee_or_impl;
template <typename C, typename... Fs>
struct tee_or_impl<true, C, Fs...> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<true_>;
};
template <typename C, typename... Fs>
struct tee_or_impl<false, C, Fs...> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<false_>;
};
template <typename C, typename F, typename... Fs>
struct tee_or_impl<false, C, F, Fs...> {
    template <typename... Ts>
    using f = tee_or_impl<
            dispatch<find_dispatch(sizeof...(Ts)), F>::template f<Ts...>::value, C,
            Fs...>::template f<Ts...>;
};
template <bool b, typename C, typename... Fs>
struct tee_or_impl_1;
template <typename C, typename... Fs>
struct tee_or_impl_1<true, C, Fs...> {
    template <typename T0>
    using f = dispatch<1, C>::template f<true_>;
};
template <typename C, typename... Fs>
struct tee_or_impl_1<false, C, Fs...> {
    template <typename T0>
    using f1 = dispatch<1, C>::template f<false_>;
};
template <typename C, typename F, typename... Fs>
struct tee_or_impl_1<false, C, F, Fs...> {
    template <typename T0>
    using f = tee_or_impl<dispatch<1, F>::template f<T0>::value, C,
                                    Fs...>::template f<T0>;
};
template <bool b, typename C, typename... Fs>
struct tee_or_impl_2;
template <typename C, typename... Fs>
struct tee_or_impl_2<true, C, Fs...> {
    template <typename T0, typename T1>
    using f = dispatch<1, C>::template f<true_>;
};
template <typename C, typename... Fs>
struct tee_or_impl_2<false, C, Fs...> {
    template <typename T0, typename T1>
    using f = dispatch<1, C>::template f<false_>;
};
template <typename C, typename F, typename... Fs>
struct tee_or_impl_2<false, C, F, Fs...> {
    template <typename T0, typename T1>
    using f = tee_or_impl<dispatch<2, F>::template f<T0, T1>::value, C,
                                    Fs...>::template f<T0, T1>;
};

template <typename N, typename L, typename C, typename... Fs>
struct tee_impl<N, L, or_<identity_, C>, Fs...> : tee_or_impl<false, C, Fs..., L> {};
template <typename L, typename C, typename... Fs>
struct tee_impl<sizet_<1>, L, or_<identity_, C>, Fs...>
        : tee_or_impl_1<false, C, Fs..., L> {};
template <typename C, typename... Fs>
struct tee_impl<sizet_<1>, identity_, or_<identity_, C>, Fs...>
        : tee_or_impl_1<false, C, Fs..., identity_> {};
template <typename L, typename C, typename... Fs>
struct tee_impl<sizet_<2>, L, or_<identity_, C>, Fs...>
        : tee_or_impl_2<false, C, Fs..., L> {};

template <std::size_t N, typename F0, typename F1, typename... Fs>
struct dispatch<N, tee_<F0, F1, Fs...>>
    : dispatch<find_dispatch(sizeof...(Fs) + 2),
                rotate_<sizet_<sizeof...(Fs)>, push_front_<sizet_<N>, lift_<tee_impl>>>>::
                template f<F0, F1, Fs...> {};

namespace tee_test {

// TODO: Narrow tests down.
// list_<list_<>>{}          = call_<tee_<listify_, listify_>>{};
// list_<list_<int>>{}       = call_<tee_<listify_, listify_>, int>{};
// list_<list_<int, bool>>{} = call_<tee_<listify_, listify_>, int, bool>{};

// true_{}  = call_<tee_<is_<int>, and_<>>, int>{};
// false_{} = call_<tee_<is_<int>, is_<int>, and_<>>, char>{};
// false_{} = call_<tee_<is_<int>, is_<int>, is_<int>, and_<>>, char>{};
// false_{} = call_<tee_<is_<int>, identity_, and_<>>,
//                     char>{}; // short circuiting should save from hard error
// false_{} = call_<tee_<is_<int>, identity_, is_<int>, and_<>>,
//                     char>{}; // short circuiting should save from hard error

// true_{}  = call_<tee_<is_<int>, or_<>>, int>{};
// false_{} = call_<tee_<is_<int>, is_<int>, or_<>>, char>{};
// false_{} = call_<tee_<is_<int>, is_<int>, is_<int>, or_<>>, char>{};
// true_{}  = call_<tee_<is_<int>, identity_, or_<>>,
//                 int>{}; // short circuiting should save from hard error
// true_{}  = call_<tee_<is_<int>, identity_, is_<int>, or_<>>,
//                 int>{}; // short circuiting should save from hard error

} // namespace tee_test

} // namespace boost::tmp

namespace boost::tmp { // chunk_

// chunk_ : Get every nth element in a parameter pack, starting
// at zero.
// Input params: Parameter pack
// Closure params: S - Positive integer type denoting the step of the stride.
//                 C - Continuation; default listify_
// Functional description:
// input  -  T0, T1, T2, T3, T4, T5, ..., TN
// apply  -  (get every nth element)
// result -  T0, T(n), T(2n), T(3n), ..., T(m * n)
// Empty return type: list_<>
export template<typename S = sizet_<0>, typename C = listify_>
struct chunk_ {};

// chunk_ : implementation
// TODO: Move this to MakeSequence module.
template <std::size_t N>
using make_index_for_stride = make_seq_impl<next_state(0, N)>::template f<N>;

consteval std::size_t chunk_div(std::size_t input_size, std::size_t chunk_length) {
    if((input_size % chunk_length < 1) && (input_size > chunk_length))
        return input_size / chunk_length;
    else if(input_size < chunk_length)
        return 1;
    else // remainder chunk
        return input_size / chunk_length + 1;
}
consteval std::size_t chunking_size(std::size_t current_index, std::size_t chunk_size, std::size_t val_count) {
    if(auto remainder = val_count % chunk_size; (remainder > 0) && ((current_index * chunk_size + remainder) == val_count))
        return remainder; // Return the last, unevenly chunked, elements.
    else
        return chunk_size;
}
template<typename S, typename C, typename... Ts>
struct chunk_impl {};
template<std::size_t N, typename S, typename C, typename... Ts>
struct dispatch<N, chunk_impl<S, list_<Ts...>, C>> {
    template<typename... Us>
    using f = dispatch<sizeof...(Us),
        tee_<window_<sizet_<(Ts::value * S::value)>,
                      sizet_<chunking_size(Ts::value, S::value, sizeof...(Us))>>..., C>>::template f<Us...>;
};
template<std::size_t N, typename S, typename C>
struct dispatch<N, chunk_<S, C>> {
    template<typename... Ts>
    using f = dispatch<sizeof...(Ts),
        chunk_impl<S, make_index_for_stride<chunk_div(sizeof...(Ts), S::value)>, C>>::template f<Ts...>;
};

namespace chunk_test {

template<typename T> requires(std::same_as<T, list_<list_<int_<0>, int_<1>, int_<2>>,
                                                    list_<int_<3>, int_<4>, int_<5>>,
                                                    list_<int_<6>, int_<7>>>>)
struct ChunkEveryThreeElements;

using chunk_test_1 = ChunkEveryThreeElements<call_<chunk_<sizet_<3>>, int_<0>, int_<1>, int_<2>, int_<3>, int_<4>, int_<5>, int_<6>, int_<7>>>;

} // namespace chunk_test

} // namespace boost::tmp

namespace boost::tmp { // slide_

// TODO: Add incrementing by a number other than 1.

// slide_ : Given a window size W, form a list of lists where each individual
// list is W elements long, starting at index 0 and incrementing until the last
// list contains the last element.
// at zero.
// Input params: Parameter pack
// Closure params: W - Positive integer type denoting the window size.
//                 C - Continuation; default listify_
// Functional description:
// input  -  T0, T1, T2, T3, T4, T5, ..., TN
// apply  -  (get W elements starting at index 0, incrementing through until the last element)
// result -  L(T0, ..., TW), L(T1, ... T(W + 1)), ..., L(T(W - N), TN)
// Empty return type: list_<>
template<typename W = sizet_<0>, typename C = listify_>
struct slide_ {};

// slide_ : implementation
// TODO: Move this to MakeSequence module.
template <std::size_t N>
using make_index_for_slide = make_seq_impl<next_state(0, N)>::template f<N>;
template<typename W, typename C = listify_, typename L = listify_>
struct slide_impl {};
template<std::size_t N, typename W, typename C, typename... Ts>
struct dispatch<N, slide_impl<W, list_<Ts...>, C>> {
    template<typename... Us>
    using f = dispatch<sizeof...(Us), tee_<window_<Ts, W>..., C>>::template f<Us...>;
};
template<std::size_t N, typename W, typename C>
struct dispatch<N, slide_<W, C>> {
    template<typename... Ts>
    using f = dispatch<N, slide_impl<W, make_index_for_slide<sizeof...(Ts) - W::value + 1>, C>>::template f<Ts...>;
};
template<typename W, typename C>
struct dispatch<0, slide_<W, C>> {
    template<typename... Ts>
    using f = dispatch<0, slide_impl<W, list_<sizet_<0>>, C>>::template f<>;
};

namespace sliding_test {

template<typename T> requires(std::same_as<T, list_<list_<int_<1>, int_<2>>,
                                                    list_<int_<2>, int_<3>>,
                                                    list_<int_<3>, int_<4>>>>)
struct SlideTwoIncrByOne;
template<typename T> requires(std::same_as<T, list_<list_<int_<1>, int_<2>, int_<3>>, 
                                                    list_<int_<2>, int_<3>, int_<4>>>>)
struct SlideThreeIncrByOne;
            
template<typename T> requires(std::same_as<T, list_<list_<int_<1>, int_<2>, int_<3>, int_<4>>>>)
struct SlideFourIncrByOne;

using slide_test_1 = SlideTwoIncrByOne<call_<slide_<sizet_<2>>, int_<1>, int_<2>, int_<3>, int_<4>>>;
using slide_test_2 = SlideThreeIncrByOne<call_<slide_<sizet_<3>>, int_<1>, int_<2>, int_<3>, int_<4>>>;
using slide_test_3 = SlideFourIncrByOne<call_<slide_<sizet_<4>>, int_<1>, int_<2>, int_<3>, int_<4>>>;

} // namespace sliding_test

} // namespace boost::tmp

namespace boost::tmp { // *_if_ functions

// count_if_ : Given a predicate F, check the variadic parameter pack passed in and count
// each time that the predicate holds true. Returns n counts as sizet_<n>.
export template <typename F, typename C = identity_>
struct count_if_ {};

// count_if_ : implementation
template <std::size_t N, typename F, typename C>
struct dispatch<N, count_if_<F, C>>
    : dispatch<N, transform_<if_<F, always_<list_<void>>, always_<list_<>>>,
                                join_<size_<C>>>> {};

// find_if_ : 
export template <typename F, typename C = identity_>
struct find_if_ {};

// find_if_ : implementation
template <bool Found, std::size_t At, template <typename...> class F>
struct county {
    static constexpr auto value{std::numeric_limits<std::size_t>::max()};
    template <typename T>
    using f = county<F<T>::value, (At + 1), F>;
};
template <std::size_t At, template <typename...> class F>
struct county<true, At, F> {
    template <typename T>
    using f                    = county;
    static constexpr std::size_t value = At;
};
template <std::size_t N, typename F, typename C>
struct dispatch<N, find_if_<F, C>> {
    template <typename... Ts>
    using f = typename dispatch<1, C>::template f<
                    typename foldey<select_foldey_loop(sizeof...(Ts))>::template f<
                    county<false,
                            std::numeric_limits<std::size_t>::max(),
                            dispatch<1, F>::template f>, 0, Ts...>>;
};
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, find_if_<lift_<F>, C>> {
        template <typename... Ts>
        using f = typename dispatch<1, C>::template f<typename foldey<select_foldey_loop(
                     sizeof...(Ts))>::template f<county<false,
                                                        std::numeric_limits<std::size_t>::max(),
                                                        F>, 0, Ts...>>;
};

// find_if_not_ : 
export template <typename F, typename C = identity_>
struct find_if_not_ {};

// find_if_not_ : implementation
template <bool Found, std::size_t At, template <typename...> class F>
struct county_not {
    static constexpr auto value{std::numeric_limits<std::size_t>::max()};
    template <typename T>
    using f = county_not<F<T>::value, (At + 1), F>;
};
template <std::size_t At, template <typename...> class F>
struct county_not<false, At, F> {
    template <typename T>
    using f                    = county_not;
    static constexpr std::size_t value = At;
};
template <std::size_t N, typename F, typename C>
struct dispatch<N, find_if_not_<F, C>> {
    template <typename... Ts>
    using f = typename dispatch<1, C>::template f<
                    typename foldey<select_foldey_loop(sizeof...(Ts))>::template f<
                    county_not<true,
                            std::numeric_limits<std::size_t>::max(),
                            dispatch<1, F>::template f>, 0, Ts...>>;
};
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, find_if_not_<lift_<F>, C>> {
        template <typename... Ts>
        using f = typename dispatch<1, C>::template f<typename foldey<select_foldey_loop(
                     sizeof...(Ts))>::template f<county_not<true,
                                                        std::numeric_limits<std::size_t>::max(),
                                                        F>, 0, Ts...>>;
};

// remove_if_ : Given a predicate F, check the variadic parameter pack passed in
// and remove the value if the predicate holds true.
export template <typename F, typename C = listify_>
struct remove_if_ {};

// remove_if_ : implementation
template<std::size_t N, typename F, typename C>
struct dispatch<N, remove_if_<F, C>>
        : dispatch<N, filter_<if_<F, always_<false_>, always_<true_>>, C>> {};

// replace_if_ : Given a variadic parameter pack, replace every value that fulfills
// the predicate F with the value Input.
export template <typename Input, typename F, typename C = listify_>
struct replace_if_ {};

// replace_if_ : implementation
template <std::size_t N, typename Input, typename F, typename C>
struct dispatch<N, replace_if_<Input, F, C>>
        : dispatch<N, transform_<if_<F, always_<Input>, identity_>, C>> {};

// TESTING:
namespace remove_if_test {
using namespace boost::tmp;

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<3>>>)
struct OnlyOddNumbersLeft;

template<typename T>
using is_even = bool_<(T::value % 2 == 0)>;


using test_one = OnlyOddNumbersLeft<call_<remove_if_<lift_<is_even>>, int_<1>, int_<2>, int_<3>>>;
} // namespace remove_if_test

// TESTING:
namespace count_if_test {
using namespace boost::tmp;

template<typename T> requires(std::same_as<T, sizet_<0>>)
struct NoEvenNumbers;

template<typename T> requires(std::same_as<T, sizet_<3>>)
struct HasThreeEvenNumbers;

template<typename T>
using is_even = bool_<(T::value % 2 == 0)>;

using test_one  = NoEvenNumbers<call_<count_if_<lift_<is_even>>, int_<1>, int_<3>>>;

using test_two  = HasThreeEvenNumbers<call_<count_if_<lift_<is_even>>, int_<0>, int_<2>, int_<4>>>;

// Empty input pack returns 0
using test_three = NoEvenNumbers<call_<count_if_<lift_<is_even>>>>;
} // namespace count_if_test

// TESTING:
namespace replace_if_test {
using namespace boost::tmp;

template<typename T> requires(std::same_as<T, list_<uint_<1>, char_<'c'>, uint_<1>>>)
struct ReplaceTwoWithC;

template<typename T> requires(std::same_as<T, list_<>>)
struct EmptyPackReturnsAnEmptyList;

template<typename T>
using is_even = bool_<(T::value % 2 == 0)>;

using test_one = ReplaceTwoWithC<call_<replace_if_<char_<'c'>, lift_<is_even>>, uint_<1>, uint_<2>, uint_<1>>>;

using test_two = EmptyPackReturnsAnEmptyList<call_<replace_if_<char_<'c'>, lift_<is_even>>>>;
} // namespace replace_if_test

// TESTING:
namespace find_if_test {
using namespace boost::tmp;

template<typename T> requires(std::same_as<T, sizet_<3>>)
struct EvenNumberAtPositionThree;

template<typename T> requires(std::same_as<T, nothing_>)
struct ReturnNothingForNoValueFound;

template<typename T>
using is_even = bool_<(T::value % 2 == 0)>;

using test_one = EvenNumberAtPositionThree<call_<find_if_<lift_<is_even>>, int_<1>, int_<1>, int_<1>, int_<2>>>;

// find_if_ returns nothing_ when there is no value found that satisfies the predicate.
using test_two = ReturnNothingForNoValueFound<call_<find_if_<lift_<is_even>>, int_<1>>>;

template<typename T> requires(std::same_as<T, sizet_<2>>)
struct OddNumberAtPositionTwo;

template<typename T> requires(std::same_as<T, nothing_>)
struct ReturnNothingForNoValueFound;

using find_if_not_test_1 = OddNumberAtPositionTwo<call_<find_if_not_<lift_<is_even>>, int_<2>, int_<4>, int_<1>, int_<2>>>;

// find_if_ returns nothing_ when there is no value found that satisfies the predicate.
using find_if_not_test_2 = ReturnNothingForNoValueFound<call_<find_if_not_<lift_<is_even>>, int_<2>>>;

} // namespace find_if_test

} // namespace boost::tmp

namespace boost::tmp { // clamp_

// clamp_ : Given a lower-bound type (L) and higher-bound type (H), and that both types
// are less than and greater than comparable, remove all types less than L and all types
// greater than H from a given VPP.
// The VPP values must also satisfy being less than and greater than comparable.
export template <typename L, typename H, typename C = identity_>
struct clamp_ {};

// clamp_ : implementation
template <std::size_t N, typename L, typename H, typename C>
struct dispatch<N, clamp_<L, H, C>>
    : dispatch<N, remove_if_<range_lo_hi_<L, H, C>>> {};

namespace clamp_test {

template<typename T> requires(std::same_as<T, list_<uint_<4>>>)
struct ListWithOnlyFour;

using test_one = ListWithOnlyFour<call_<clamp_<uint_<3>, uint_<10>>, uint_<0>, uint_<1>, uint_<2>, uint_<3>, uint_<4>>>;

} // namespace clamp_test

} // namespace boost::tmp

namespace boost::tmp { // product_

//  product_ : Given two lists, generates the Cartesian product of said lists (n x m tuples generated).
// 	using xl0 = list_<x<1>, x<2>, ..., x<n>>;
//	using xr0 = list_<x<1>, x<2>, ..., x<m>>;
//
//  using result = call_<product_<>, xl0, xr0>;
//  \text Will generate the Cartesian product of the lists xl0 and xr0, from xl0[1]-xl0[n]
//  and xr0[1]-xr0[m]: result = list_<list_<x<1>, x<1>>, list_<x<1>, x<2>>, ..., list_<x<1>,
//  x<n>>, ..., list_<x<n>, x<1>>, list_<x<n>, x<2>>, ..., list_<x<n>, x<m>>>
export template <typename F = listify_, typename C = listify_>
struct product_ {};

// product_ : implementation
template <template <typename...> class F, typename T, typename... Ts>
using product_helper_ = list_<F<T, Ts>...>;
template <template <typename...> class F, typename C, typename T, typename U>
struct product_unpacker {};
template <template <typename...> class F, typename C, typename... Ts, typename... Us>
struct product_unpacker<F, C, list_<Ts...>, list_<Us...>> {
    using type = dispatch<find_dispatch(sizeof...(Ts)),
                                join_<C>>::template f<product_helper_<F, Ts, Us...>...>;
};
template <template <typename...> class F, typename C>
struct dispatch<2, product_<lift_<F>, C>> {
    template <typename T, typename U>
    using f = product_unpacker<F, C, T, U>::type;
};
template <typename F, typename C>
struct dispatch<2, product_<F, C>> : dispatch<2, product_<lift_<dispatch<2, F>::template f>, C>> {};

namespace product_test {

// TODO: Implement product_test

// using xl1 = list_<uint_<1>, uint_<2>, uint_<3>>;
// using xr1 = list_<uint_<1>, uint_<2>, uint_<3>>;

// using result1 = call_<product_<>, xl1, xr1>;

// list_<list_<uint_<1>, uint_<1>>, list_<uint_<1>, uint_<2>>, list_<uint_<1>, uint_<3>>,
//         list_<uint_<2>, uint_<1>>, list_<uint_<2>, uint_<2>>, list_<uint_<2>, uint_<3>>,
//         list_<uint_<3>, uint_<1>>, list_<uint_<3>, uint_<2>>, list_<uint_<3>, uint_<3>>>{} =
//         result1{};

} // namespace product_test

} // namespace boost::tmp

namespace boost::tmp { // partition_

// partition_ : Given a unary predicate, separate a VPP into a list of two lists,
// with the first list being the elements where the predicate is true.
// Maintains order of elements.
export template <typename F, typename C = listify_>
struct partition_ {};

// partition_ : implementation
template <std::size_t N, typename F, typename C>
struct dispatch<N, partition_<F, C>> : dispatch<N, tee_<filter_<F>, remove_if_<F>, C>> {};

namespace partition_test {

template<typename T> requires(std::same_as<T, list_<list_<int_<3>, int_<4>>, list_<int_<1>, int_<2>>>>)
struct SplitIntoTwoListWithGreaterThanTwo;

template <typename T>
using greater_than_two = bool_<(T::value > 2)>;

using test_one = SplitIntoTwoListWithGreaterThanTwo<call_<partition_<lift_<greater_than_two>>, int_<1>, int_<2>, int_<3>, int_<4>>>;

} // namespace partition_test

} // namespace boost::tmp

namespace boost::tmp { // sort_
// NOTE: Sort only works on lists, not parameter packs.

// sort_ :
export template <typename F = less_<>, typename C = listify_>
struct sort_ {};

// sort_ : implementation
namespace btree {
    // The binary tree as a list.
    template <typename... Ts>
    struct blist {};

    // A node in the binary tree, connecting to other nodes (L)eft and (R)ight, and (E)dge
    template <typename L, typename E, typename R>
    struct bnode {};
    template <typename T>
    struct bun;
    template <bool, template <typename...> class F>
    struct bpush;
    template <template <typename...> class F>
    struct bpush<false, F> {
        template <typename T, typename L, typename E, typename R>
        using n = bnode<L, E, typename bun<R>::template f<F, T>>;
        template<typename T, typename LL, typename LE, typename LR,
                    typename E, typename RL, typename RE, typename RR>
        using nn = bpush<F<T, RE>::value, F>::template nn_0<T, LL, LE, LR, E, RL, RE, RR>;
        template <typename T, typename LL, typename LE, typename LR, typename E,
                    typename RL, typename RE, typename RR>
        using nn_0 = bnode<bnode<bnode<LL, LE, LR>, E, RL>, RE,
                            typename bun<RR>::template f<F, T>>;
        template <typename T, typename LL, typename LE, typename LR,
                    typename E, typename RL, typename RE, typename RR>
        using nn_1 = bnode<bnode<LL, LE, typename bun<LR>::template f<F, T>>, E,
                            bnode<RL, RE, RR>>;
        template <typename T, typename E>
        using l1 = blist<E, T>;
        template <typename T, typename E0, typename E1>
        using l2_0 = bpush<F<T, E1>::value, F>::template l2_1<T, E0, E1>;
        template <typename T, typename E0, typename E1>
        using l2_1 = bnode<blist<E0>, E1, blist<T>>;
    };
    template <template <typename...> class F>
    struct bpush<true, F> {
        template <typename T, typename L, typename E, typename R>
        using n = bnode<typename bun<L>::template f<F, T>, E, R>;
        template <typename T, typename LL, typename LE, typename LR,
                    typename E, typename RL, typename RE, typename RR>
        using nn = bpush<F<T, LE>::value, F>::template nn_1<T, LL, LE, LR, E, RL, RE, RR>;
        template <typename T, typename LL, typename LE, typename LR,
                    typename E, typename RL, typename RE, typename RR>
        using nn_0 = bnode<bnode<LL, LE, LR>, E,
                            bnode<typename bun<RL>::template f<F, T>, RE, RR>>;
        template <typename T, typename LL, typename LE, typename LR,
                    typename E, typename RL, typename RE, typename RR>
        using nn_1 = bnode<typename bun<LL>::template f<F, T>, LE,
                            bnode<LR, E, bnode<RL, RE, RR>>>;
        template <typename T, typename E>
        using l1 = blist<T, E>;
        template <typename T, typename E0, typename E1>
        using l2_0 = bnode<blist<T>, E0, blist<E1>>;
        template <typename T, typename E0, typename E1>
        using l2_1 = bnode<blist<E0>, T, blist<E1>>;
    };

    template <>
    struct bun<blist<>> {
        template <template <typename...> class F, typename T>
        using f = blist<T>;
    };
    template <typename E>
    struct bun<blist<E>> {
        template <template <typename...> class F, typename T>
        using f = bpush<F<T, E>::value, F>::template l1<T, E>;
    };
    template <typename E0, typename E1>
    struct bun<blist<E0, E1>> {
        template <template <typename...> class F, typename T>
        using f = bpush<F<T, E0>::value, F>::template l2_0<T, E0, E1>;
    };

    // Push a node. The node postion (either Left or Right) from the Edge
    // is determined by the type bpush<F<T, E>::value, F>::template
    // and will dispatch the correct node position by calling each
    // version's respective n<T, L, E, R> function.
    template <typename L, typename E, typename R>
    struct bun<bnode<L, E, R>> {
        template <template <typename...> class F, typename T>
        using f = bpush<F<T, E>::value, F>::template n<T, L, E, R>;
    };
    template <typename LL, typename LE, typename LR, typename E, typename RL,
                typename RE, typename RR>
    struct bun<bnode<bnode<LL, LE, LR>, E, bnode<RL, RE, RR>>> {
        template <template <typename...> class F, typename T>
        using f = bpush<F<T, E>::value, F>::template nn<T, LL, LE, LR, E, RL,
                                                                    RE, RR>;
    };

    template <typename LLL, typename LLE, typename LLR, typename LE, typename LR,
                typename E, typename... Ts>
    struct bun<bnode<bnode<bnode<LLL, LLE, LLR>, LE, LR>, E, blist<Ts...>>> {
        template <template <typename...> class F, typename T>
        using f = bpush<F<T, LE>::value, F>::template nn<T, LLL, LLE, LLR, LE,
                                                                    LR, E, blist<Ts...>>;
    };

    template <typename... Ts, typename E, typename RL, typename RE, typename RRL,
                typename RRE, typename RRR>
    struct bun<bnode<blist<Ts...>, E, bnode<RL, RE, bnode<RRL, RRE, RRR>>>> {
        template <template <typename...> class F, typename T>
        using f = bpush<F<T, RE>::value, F>::template nn<T, blist<Ts...>, E, RL,
                                                            RE, RRL, RRE, RRR>;
    };

    // Prune branch

    // Remove leaf

    // Remove 

    template <typename LL, typename LE, typename LR, typename E, typename RL,
                typename RE, typename RR>
    struct join7;

    template <typename... LL, typename LE, typename... LR, typename E, typename... RL,
                typename RE, typename... RR>
    struct join7<blist<LL...>, LE, blist<LR...>, E, blist<RL...>, RE, blist<RR...>> {
        using type = blist<LL..., LE, LR..., E, RL..., RE, RR...>;
    };

    // Binary tree flattener metaclosures
    template <typename T>
    struct bflat;

    // Flatten single end node with Left, Edge, and Right
    template <typename L, typename E, typename R>
    struct bflat<bnode<L, E, R>> {
        using type = bflat<bnode<typename bflat<L>::type, E, typename bflat<R>::type>>::type;
    };

    // Flatten two nodes, Left and Right from Edge, and flatten their respective Left
    // and Right nodes with their Edges. Join the seven resulting types into
    // a blist, respecting their order.
    template <typename LL, typename LE, typename LR, typename E, typename RL,
                typename RE, typename RR>
    struct bflat<bnode<bnode<LL, LE, LR>, E, bnode<RL, RE, RR>>> {
        using type = join7<typename bflat<LL>::type, LE, typename bflat<LR>::type,
                                E, 
                                typename bflat<RL>::type, RE, typename bflat<RR>::type
                            >::type;
    };

    // From a node, combine the Left and Right chlidren 
    // into a single blist.
    template <typename... L, typename E, typename... R>
    struct bflat<bnode<blist<L...>, E, blist<R...>>> {
        using type = blist<L..., E, R...>;
    };

    // Final combination of all the resulting types.
    template <typename... T>
    struct bflat<blist<T...>> {
        using type = blist<T...>;
    };

    template <typename T>
    using collapse_t = btree::bflat<T>::type;
} // namespace btree

// Push elements to btree
template <typename F>
struct element_pusher {};
template <typename F>
struct dispatch<2, element_pusher<F>> {
    template <typename T, typename U>
    using f = btree::bun<T>::template f<dispatch<2, F>::template f, U>;
};
template <template <typename...> class F>
struct dispatch<2, element_pusher<lift_<F>>> {
    template <typename T, typename U>
    using f = btree::bun<T>::template f<F, U>;
};
template <template <typename...> class F>
struct pusher {
    template <typename T, typename U>
    using f = btree::bun<T>::template f<F, U>;
};
template <template <typename...> class F, typename C = identity_,
            typename Initial = btree::blist<>>
using make_binary_tree =
        push_front_<Initial, fold_left_<lift_<pusher<F>::template f>, C>>;
template <typename C>
using collapse_unpack = lift_<btree::collapse_t, unpack_<C>>;
template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, sort_<lift_<F>, C>>
    : dispatch<N, make_binary_tree<F, collapse_unpack<C>, btree::blist<>>> {};
template <std::size_t N, typename F, typename C>
struct dispatch<N, sort_<F, C>>
    : dispatch<N, make_binary_tree<dispatch<2, F>::template f, collapse_unpack<C>,
                                    btree::blist<>>> {};

namespace sort_test {

template<typename T> requires(std::same_as<T, list_<sizet_<0>, sizet_<1>, sizet_<2>>>)
struct ListZeroOneTwo;

template <typename T, typename U>
using less = bool_<(T::value < U::value)>;

using test_one = ListZeroOneTwo<call_<sort_<lift_<less>>, sizet_<1>, sizet_<0>, sizet_<2>>>;

} // namespace sort_test

} // namespace boost::tmp

namespace boost::tmp { // split_

template<typename I = sizet_<0>, typename C = listify_>
struct split_ {};

template<std::size_t N, typename I, typename C>
struct dispatch<N, split_<I, C>> : dispatch<N, tee_<take_<I>, drop_<I>, listify_>> {};

namespace split_test {

template<typename T> requires(std::same_as<T, list_<list_<int_<1>, int_<2>, int_<3>, int_<4>>,
                                                    list_<int_<5>, int_<6>, int_<7>, int_<8>>>>)
struct SplitAtIndexFour;

using split_test_1 = SplitAtIndexFour<call_<split_<sizet_<4>>, int_<1>, int_<2>, int_<3>, int_<4>,
                                                               int_<5>, int_<6>, int_<7>, int_<8>>>;

} // namespace split_test

} // namespace boost::tmp

namespace boost::tmp { // drop_while_ / drop_while_back_ / trim_

// drop_list_ :
template<typename C = listify_>
struct drop_list_ {};

// drop_list_ : implementation
template<std::size_t N, typename C>
struct dispatch<N, drop_list_<C>> {
    template<typename DropValue, typename... Ts>
    using f = dispatch<find_dispatch(sizeof...(Ts)), unpack_<drop_<DropValue, C>>>::template f<Ts...>;
};

// drop_while_ :
export template<typename F, typename C = listify_>
struct drop_while_ {};

// drop_while_ : implementation
template<std::size_t N, typename F, typename C>
struct dispatch<N, drop_while_<F, C>> : dispatch<N, tee_<find_if_not_<F>, listify_, drop_list_<C>>> {};

// trim_ :
export template<typename F, typename C = listify_>
struct trim_ {};

// trim_ : implementation
template<std::size_t N, typename F, typename C>
struct dispatch<N, trim_<F, C>> : dispatch<N, drop_while_<F, reverse_<drop_while_<F, reverse_<C>>>>> {};

// drop_while_back_ :
export template<typename F, typename C = listify_>
struct drop_while_back_ {};

// drop_while_back_ : implementation
template<std::size_t N, typename F, typename C>
struct dispatch<N, drop_while_back_<F, C>> : dispatch<N, reverse_<drop_while_<F, reverse_<C>>>> {};

namespace drop_while_test {

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<1>>>)
struct DropEvenNumbersAtFront;

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<1>>>)
struct DropEvenNumbersAtBack;

template<typename T>
using is_even = bool_<(T::value % 2 == 0)>;

using drop_while_test_1 = DropEvenNumbersAtFront<call_<drop_while_<lift_<is_even>>, int_<2>, int_<2>, int_<1>, int_<1>>>;

using drop_while_back_test_1 = DropEvenNumbersAtBack<call_<drop_while_back_<lift_<is_even>>, int_<1>, int_<1>, int_<2>, int_<2>>>;

template<typename T> requires(std::same_as<T, list_<int_<1>, int_<6>, int_<1>>>)
struct TrimEvenNumbersUntilOdd;

template<typename T>
using is_even = bool_<(T::value % 2 == 0)>;

using trim_test_1 = TrimEvenNumbersUntilOdd<call_<trim_<lift_<is_even>>, int_<2>, int_<1>, int_<6>, int_<1>, int_<2>>>;

} // drop_while_test

} // namespace boost::tmp

namespace boost::tmp { // stride_

// stride_ : Get every nth element in a parameter pack, starting
// at zero.
// Input params: Parameter pack
// Closure params: S - Positive integer type denoting the step of the stride.
//                 C - Continuation; default listify_
// Functional description:
// input  -  T0, T1, T2, T3, T4, T5, ..., TN
// apply  -  (get every nth element)
// result -  T0, T(n), T(2n), T(3n), ..., T(m * n)
// Empty return type: list_<>
template<typename S = sizet_<0>, typename C = listify_>
struct stride_ {};

// TODO: Move this to MakeSequence module.
template <std::size_t N>
using make_index_for_stride = make_seq_impl<next_state(0, N)>::template f<N>;

// stride_ : implementation
template<typename S, typename C = listify_, typename L = listify_>
struct tee_drops_ {};
template<std::size_t N, typename S, typename C, typename... Ts>
struct dispatch<N, tee_drops_<S, list_<Ts...>, C>> {
    template<typename... Us>
    using f = dispatch<sizeof...(Us) / S::value + 1, tee_<index_<sizet_<Ts::value * S::value>>..., C>>::template f<Us...>;
};
template<typename S, typename C, typename... Ts>
struct dispatch<0, tee_drops_<S, list_<Ts...>, C>> {
    template<typename... Us>
    using f = dispatch<0, tee_<i0_<>, C>>::template f<Us...>;
};

consteval std::size_t stride_div(std::size_t input_size, std::size_t stride_length) {
    return (input_size % stride_length) < 1
        ? input_size / stride_length
        : input_size / stride_length + 1;
}

template<std::size_t N, typename S, typename C>
struct dispatch<N, stride_<S, C>> {
    template<typename... Ts>
    using f = dispatch<N, tee_drops_<S, make_index_for_stride<stride_div(sizeof...(Ts), S::value)>, C>>::template f<Ts...>;
};
template<typename S, typename C>
struct dispatch<0, stride_<S, C>> {
    template<typename... Ts>
    using f = dispatch<0, tee_drops_<S, list_<sizet_<0>>, C>>::template f<>;
};

namespace stride_test {

template<typename T> requires(std::same_as<T, list_<int_<0>, int_<1>, int_<2>, int_<3>, int_<4>, int_<5>>>)
struct EveryZerothElement;

template<typename T> requires(std::same_as<T, list_<int_<0>, int_<2>, int_<4>>>)
struct EverySecondElement;

template<typename T> requires(std::same_as<T, list_<char_<'a'>, char_<'d'>, char_<'g'>>>)
struct EveryThirdElement;

using stride_test_2 = EveryThirdElement<call_<stride_<sizet_<3>>, char_<'a'>, char_<'b'>, char_<'c'>, char_<'d'>, char_<'e'>, char_<'f'>, char_<'g'>>>;

} // namespace stride_test

} // namespace boost::tmp

namespace boost::tmp { // starts_with_ / ends_with_

// TODO: Add special cases:
// Comparison list is greater than the input list
// No input
// Empty comparison list
// starts_with_ :
export template<typename L, typename C = identity_>
struct starts_with_ {};

template<std::size_t N, typename L, typename C = identity_>
struct starts_with_impl;

template<std::size_t N, typename C, typename... Ts>
struct dispatch<N, starts_with_impl<N, list_<Ts...>, C>> {
    template<typename... Us>
    using f = dispatch<sizeof...(Us),
        take_<sizet_<sizeof...(Ts)>,
            lift_<list_, is_<list_<Ts...>, C>>>>::template f<Us...>;
};

template<std::size_t N, typename L, typename C>
struct dispatch<N, starts_with_<L, C>> : dispatch<N, starts_with_impl<N, L, C>> {};

// ends_with :
export template<typename L, typename C = identity_>
struct ends_with_ {};

template<std::size_t N, typename L, typename C = identity_>
struct ends_with_impl;

template<std::size_t N, typename C, typename... Ts>
struct dispatch<N, ends_with_impl<N, list_<Ts...>, C>> {
    template<typename... Us>
    using f = dispatch<sizeof...(Us),
        reverse_<
            take_<sizet_<sizeof...(Ts)>,
                reverse_<lift_<list_, is_<list_<Ts...>, C>>>>>>::template f<Us...>;
};

template<std::size_t N, typename L, typename C>
struct dispatch<N, ends_with_<L, C>> : dispatch<N, ends_with_impl<N, L, C>> {};

namespace start_end_tests {

template<typename T> requires(std::same_as<T, true_>)
struct StartsWithAB;

template<typename T> requires(std::same_as<T, false_>)
struct DoesNotStartWithAD;

template<typename T> requires(std::same_as<T, true_>)
struct EndsWithBD;

template<typename T> requires(std::same_as<T, false_>)
struct DoesNotEndWithAD;

using starts_with_test_1 = StartsWithAB<call_<starts_with_<list_<char_<'A'>, char_<'B'>>>, char_<'A'>, char_<'B'>, char_<'D'>>>;
using starts_with_test_2 = DoesNotStartWithAD<call_<starts_with_<list_<char_<'A'>, char_<'D'>>>, char_<'A'>, char_<'B'>, char_<'D'>>>;

using ends_with_test_1 = EndsWithBD<call_<starts_with_<list_<char_<'A'>, char_<'B'>>>, char_<'A'>, char_<'B'>, char_<'D'>>>;
using ends_with_test_2 = DoesNotEndWithAD<call_<starts_with_<list_<char_<'A'>, char_<'D'>>>, char_<'A'>, char_<'B'>, char_<'D'>>>;

} // namespace start_end_tests

} // namespace boost::tmp

namespace boost::tmp { // unique_

template <typename T>
struct unique_magic_type;

// Our starting point; gets dropped off at the end.
struct unique_super_base {
    static consteval bool contains(void *) {
        return false;
    }
};
template <typename Tail, typename T>
struct unique_base : Tail {
    static consteval bool contains(unique_magic_type<T> *) {
        return true;
    }

    using Tail::contains;
};

template <typename T, typename U>
using is_in_set = bool_<T::contains(static_cast<unique_magic_type<U> *>(0))>;

using unique_push_if = if_<lift_<is_in_set>, front_<>, lift_<unique_base>>;

template<typename F = identity_, typename C = listify_>
struct unique_ {};

template<std::size_t N, typename F, typename C>
struct dispatch<N, unique_<F, C>> {
    template<typename... Ts>
    using f = dispatch<N, push_front_<unique_super_base, fold_left_<unique_push_if, flatten_<drop_<uint_<1>, C>>>>>::template f<Ts...>;
};

namespace unique_test {

template<typename T> requires(std::same_as<T, list_<int_<3>, int_<1>, int_<2>>>)
struct OnlyUniqueNums;

using unique_test_1 = OnlyUniqueNums<call_<unique_<>, int_<3>, int_<1>, int_<2>, int_<2>, int_<1>, int_<2>>>;

} // namespace unique_test

} // namespace boost::tmp

namespace boost::tmp { // try_

export template <template <typename...> class F, typename C = identity_>
struct try_ {};

template <typename T>
struct t_ {
    using type = T;
};
template <template <typename...> class F, typename... Ts>
t_<F<Ts...>> try_f(lift_<F>, list_<Ts...>);
t_<nothing_> try_f(...);

template <std::size_t N, template <typename...> class F, typename C>
struct dispatch<N, try_<F, C>> {
    template <typename... Ts>
    using f = dispatch<1, C>::template f<typename decltype(
            try_f(lift_<F>{}, list_<Ts...>{}))::type>;
};

namespace try_test {

// TODO: Implement try_test
// template <typename T>
// using call_type = T::type;

// struct has_type {
//     using type = int;
// };

// nothing_{}   = call_<try_<call_type>, int>{}; // should SFINAE, int has no ::type
// list_<int>{} = list_<call_<try_<call_type>, has_type>>{}; // should not SFINAE
// list_<int>{} = call_<try_<call_type, listify_>, has_type>{}; // test the continuation

} // namespace try_test

} // namespace boost::tmp
