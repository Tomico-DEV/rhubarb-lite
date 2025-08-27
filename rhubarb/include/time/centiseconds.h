#pragma once

#include <chrono>
#include <ostream>
#include <format>

using centiseconds = std::chrono::duration<int, std::centi>;

// Needs to be in the same namespace as std::chrono::duration, or googletest won't pick it up.
// See https://github.com/google/googletest/blob/master/docs/advanced.md#user-content-teaching-googletest-how-to-print-your-values
namespace std {

	std::ostream& operator <<(std::ostream&, centiseconds cs);
	
}

template <>
struct std::formatter<centiseconds> : std::formatter<std::string> {
	constexpr auto parse(std::format_parse_context& ctx) {
        return std::formatter<std::string>::parse(ctx);
    }
	
	template <typename FormatContext>
    inline auto format(const centiseconds& cs, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << cs;
        return std::formatter<std::string>::format(oss.str(), ctx);
    }
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4455)
#endif
inline constexpr centiseconds operator "" _cs(unsigned long long cs) {
	return centiseconds(cs);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif