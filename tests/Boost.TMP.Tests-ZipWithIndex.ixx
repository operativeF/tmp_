//  Copyright 2018-2019 Odin Holmes.
//            2022 Thomas Figueroa.
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

export module Boost.TMP.Tests:Extrema;

import Boost.TMP;

namespace zip_with_index_test {
	using namespace boost::tmp;

	template <typename T>
	using doubleify = list_<T, T>;

	export int run() {
		return 0;
	}
} // namespace zip_with_index_test