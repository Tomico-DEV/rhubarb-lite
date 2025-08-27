#pragma once

#include "tools/EnumConverter.h"

namespace logging {

	enum class Level {
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Fatal,
		EndSentinel
	};

	class LevelConverter : public EnumConverter<Level> {
	public:
		static LevelConverter& get();
	protected:
		std::string getTypeName() override;
		member_data getMemberData() override;
	};

	std::ostream& operator<<(std::ostream& stream, Level value);

	std::istream& operator >>(std::istream& stream, Level& value);

}

template <>
struct std::formatter<logging::Level> : std::formatter<std::string> {
	constexpr auto parse(std::format_parse_context& ctx) {
        return std::formatter<std::string>::parse(ctx);
    }
	
	inline auto format(const logging::Level& lvl, format_context& ctx) const
	-> format_context::iterator
	{
		std::ostringstream oss;
		oss << lvl;
		return std::formatter<std::string>::format(oss.str(), ctx);
	}
};
