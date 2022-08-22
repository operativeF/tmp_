//  Copyright 2018 Odin Holmes.
//            2021 Thomas Figueroa.
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

export module Boost.TMP:Algorithm.Unpack;

import :Base.Types;

// \brief turns a list of types into a variadic pack of those types /
// example: call<all<>,true_,false_,true_> is equivalent to
// call<unpack<all<>>,list<true_,false_,true_>>
// \requirement
// Unpack always needs a continuation, so even if you're just unpacking
// a list, you need to use it like the following:
// using alist = list_<uint_<0>>;
// uint_<0>{} = call_<unpack_<identity_>, alist>{}; // This will be vaild
namespace boost::tmp {

export template <typename C>
struct unpack_ {};

namespace impl {

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

} // namespace impl

#ifdef TMP_COMPILE_TIME_TESTING
namespace test {

} // namespace test
#endif // TMP_COMPILE_TIME_TESTING

} // namespace boost::tmp
