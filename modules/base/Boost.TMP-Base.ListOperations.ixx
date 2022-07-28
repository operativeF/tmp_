//  Copyright 2018-2019 Odin Holmes.
//            2020-2022 Thomas Figueroa.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

export module Boost.TMP:Base.ListOperations;

import :Algorithm.Contains;
import :Algorithm.FindIf;
import :Algorithm.FoldLeft;
import :Algorithm.Sort;

import :Base.Call;
import :Base.Comparison;
import :Base.Identity;
import :Base.If;
import :Base.Lift;
import :Base.Logic;
import :Base.Vocabulary;

import :Sequence.Erase;
import :Sequence.Index;
import :Sequence.PushFront;
import :Sequence.Size;
import :Sequence.Unpack;

export namespace boost::tmp {
	template <typename T>
	using is_empty = call_<unpack_<size_<is_<sizet_<0>>>>, T>;

	template <typename T, typename U>
	using get_index_from_value = call_<unpack_<find_if_<is_<T>>>, U>;

	template <typename T, typename I>
	using erase_at = call_<unpack_<erase_<get_index_from_value<I, T>>>, T>;

	template <typename T, typename U>
	using contains_swap = call_<unpack_<contains_<U>>, T>;

	template <typename T, typename TPow>
	using new_unit_ = list_<T, TPow>;

	template <typename T, typename U>
	using less = bool_<(T::value < U::value)>;

	template <typename T>
	using sorter_ = call_<unpack_<sort_<lift_<less>>>, T>;

	template <typename Num, typename Den>
	using divide_lists = call_<unpack_<push_front_<Num, fold_left_<if_<lift_<contains_swap>, lift_<erase_at>, front_<>>>>>, Den>;

	template <typename Num, typename Den>
	using full_divide_lists = list_<sorter_<divide_lists<Num, Den>>, sorter_<divide_lists<Den, Num>>>;
} // namespace boost::tmp