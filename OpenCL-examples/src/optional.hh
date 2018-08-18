#pragma once

#if defined __GNUC__ && __GNUC__ > 7 // supports std::optional from C++17

#include <optional>

#else // presumably MacOS

#include <experimental/optional>

namespace std {
    using std::experimental::optional;
    using std::experimental::nullopt;
}
#endif
