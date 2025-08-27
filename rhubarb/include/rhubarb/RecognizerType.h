#pragma once

#include "tools/EnumConverter.h"
#include <sstream>

enum class RecognizerType {
	PocketSphinx,
	Phonetic
};

class RecognizerTypeConverter : public EnumConverter<RecognizerType> {
public:
	static RecognizerTypeConverter& get();
protected:
	std::string getTypeName() override;
	member_data getMemberData() override;
};

std::ostream& operator<<(std::ostream& stream, RecognizerType value);

std::istream& operator>>(std::istream& stream, RecognizerType& value);

template <>
struct fmt::formatter<RecognizerType> : fmt::formatter<std::string> {
	inline auto format(const RecognizerType& value, format_context& ctx) const
	-> format_context::iterator
	{
		std::ostringstream oss;
		oss << value;
		return fmt::formatter<std::string>::format(oss.str(), ctx);
	}
};
