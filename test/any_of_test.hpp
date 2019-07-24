//  Copyright 2018-2019 Odin Holmes.
//						Thomas Figueroa.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <boost/tmp.hpp>
#include "test_util.hpp"

namespace any_of_test {
	
    template <typename T>
	using is_even = bool_<(T::value % 2 == 0)>;

    using alist = list_<int_<1>, int_<1>>;

    template<typename F, typename C = listify_>
    using erase_if_ = transform_<if_<F, erase_<>, listify_>, join_<C>>;

    using blist = call_<unpack_<erase_if_<lift_<is_even>>>, alist>;

    using clist = call_<join_<>, call_<unpack_<erase_if_<lift_<is_even>>>, alist>>;

    // This is important. For flatten_ to work properly here, we have to join the results, otherwise they're all separate and can't be represented
    // by the type system.
    using dlist = call_<product_<flatten_<>, join_<>>, alist, alist>;

	int run() {

        blist{} = list_<int_<1>, int_<1>>{};
        clist{} = list_<int_<1>, int_<1>>{};

        dlist{} = list_<int_<1>, int_<1>, int_<1>, int_<1>, int_<1>, int_<1>, int_<1>, int_<1>>{};

        //bool_<true>{} = call_<unpack_<any_of_<lift_<is_even>>>, alist>{};
		//bool_<false>{} = call_<unpack_<any_of_<lift_<is_even>>>, blist>{};
        //bool_<true>{} = call_<unpack_<any_of_<lift_<is_even>>>, clist>{};

        return 0;
	}
} // namespace any_of_test