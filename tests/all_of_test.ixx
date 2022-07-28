//  Copyright 2018-2019 Odin Holmes.
//			  2022 Thomas Figueroa.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

module;

#ifdef __GNUC__
#include <type_traits>
#endif // __GNUC__

export module Test.AllOf;

import Boost.TMP;

#if _MSC_VER
import std;
#endif

using namespace boost::tmp;

export namespace all_of_test {
	int run() {
		return 0;
	}
} // namespace all_of_test