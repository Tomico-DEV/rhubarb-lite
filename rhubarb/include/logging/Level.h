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
struct fmt::formatter<logging::Level> : fmt::formatter<std::string> {
	inline auto format(const logging::Level& lvl, format_context& ctx) const
	-> format_context::iterator
	{
		std::ostringstream oss;
		oss << lvl;
		return fmt::formatter<std::string>::format(oss.str(), ctx);
	}
};
