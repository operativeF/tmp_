//  Copyright 2018-2019 Odin Holmes.
//                      Thomas Figueroa.
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <boost/tmp.hpp>
#include "test_util.hpp"

namespace swap_test {
	int run() {
		using alist = list_<uint_<0>, uint_<1>>;

		using alist_swap = call_<unpack_<swap_<>>, alist>;

		list_<uint_<1>, uint_<0>>{} = alist_swap{};

		return 0;
	}
} // namespace swap_test