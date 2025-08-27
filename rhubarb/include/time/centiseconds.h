#pragma once

#include <chrono>
#include <ostream>
#include <fmt/core.h>

using centiseconds = std::chrono::duration<int, std::centi>;

// Needs to be in the same namespace as std::chrono::duration, or googletest won't pick it up.
// See https://github.com/google/googletest/blob/master/docs/advanced.md#user-content-teaching-googletest-how-to-print-your-values
namespace std {

	std::ostream& operator <<(std::ostream&, centiseconds cs);
	
}

template <>
struct fmt::formatter<centiseconds> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const centiseconds& cs, FormatContext& ctx) {
        std::ostringstream oss;
        oss << cs;
        return fmt::formatter<std::string>::format(oss.str(), ctx);
    }
};

#pragma warning(push)
#pragma warning(disable: 4455)
inline constexpr centiseconds operator "" _cs(unsigned long long cs) {
	return centiseconds(cs);
}
#pragma warning(pop)
