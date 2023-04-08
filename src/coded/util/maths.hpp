#pragma once

#include <concepts>
#include <type_traits>

namespace maths {

template<std::integral I>
[[nodiscard]] constexpr I safe_subtract(I left, I right) {
    return left >= right ? left - right : 0;
}

}
