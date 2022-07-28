//  Copyright 2018-2019 Odin Holmes.
//            2022 Thomas Figueroa.
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

export module Boost.TMP.Tests:Erase;

import Boost.TMP;

namespace erase_test {
	using namespace boost::tmp;

	using xs0 = list_<int_<1>, int_<2>, int_<3>, int_<4>, int_<5>, int_<6>, int_<7>, int_<8>, int_<9>, int_<10>>;

	using result0 = call_<unpack_<erase_<sizet_<0>>>, xs0>;

	export int run() {
		result0{} = list_<int_<2>, int_<3>, int_<4>, int_<5>, int_<6>, int_<7>, int_<8>, int_<9>, int_<10>>{};

		return 0;
	}
} // namespace erase_test