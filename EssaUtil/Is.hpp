#pragma once

#include <type_traits>

namespace Util {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
template<class ExpectedT, class T>
    requires(std::is_base_of_v<T, ExpectedT>) bool
is(T const& t) {
    if constexpr (std::is_same_v<T, ExpectedT>)
        return true;

    return dynamic_cast<ExpectedT const*>(&t);
}
#pragma GCC diagnostic pop

}
