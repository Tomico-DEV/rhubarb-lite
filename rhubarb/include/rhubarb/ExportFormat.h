#pragma once

#include "tools/EnumConverter.h"
#include <sstream>

enum class ExportFormat {
	Dat,
	Tsv,
	Xml,
	Json
};

class ExportFormatConverter : public EnumConverter<ExportFormat> {
public:
	static ExportFormatConverter& get();
protected:
	std::string getTypeName() override;
	member_data getMemberData() override;
};

std::ostream& operator<<(std::ostream& stream, ExportFormat value);

std::istream& operator>>(std::istream& stream, ExportFormat& value);

template <>
struct std::formatter<ExportFormat> : std::formatter<std::string> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return std::formatter<std::string>::parse(ctx);
    }

	inline auto format(const ExportFormat& value, format_context& ctx) const
	-> format_context::iterator
	{
		std::ostringstream oss;
		oss << value;
		return std::formatter<std::string>::format(oss.str(), ctx);
	}
};
