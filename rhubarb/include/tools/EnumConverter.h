#pragma once

#include <utility>
#include <map>
#include <vector>
#include <optional>
#include <algorithm>
#include <cctype>
#include <string>
#include <stdexcept>
#include <format.h>

template<typename TEnum>
class EnumConverter {
public:
    EnumConverter() :
        initialized(false)
    {}

    virtual ~EnumConverter() = default;

    virtual std::optional<std::string> tryToString(TEnum value) {
        initialize();
        auto it = valueToNameMap.find(value);
        return it != valueToNameMap.end()
            ? std::make_optional(it->second)
            : std::nullopt;
    }

    std::string toString(TEnum value) {
        initialize();
        auto result = tryToString(value);
        if (!result) {
            auto numericValue = static_cast<typename std::underlying_type<TEnum>::type>(value);
            throw std::invalid_argument(
                fmt::format("{} is not a valid {} value.", numericValue, typeName)
            );
        }

        return *result;
    }

    virtual std::optional<TEnum> tryParse(const std::string& s) {
        initialize();
        auto it = lowerCaseNameToValueMap.find(to_lower_copy(s));
        return it != lowerCaseNameToValueMap.end()
            ? std::make_optional(it->second)
            : std::nullopt;
    }

    TEnum parse(const std::string& s) {
        initialize();
        auto result = tryParse(s);
        if (!result) {
            throw std::invalid_argument(fmt::format("{} is not a valid {} value.", s, typeName));
        }

        return *result;
    }

    std::ostream& write(std::ostream& stream, TEnum value) {
        return stream << toString(value);
    }

    std::istream& read(std::istream& stream, TEnum& value) {
        std::string name;
        stream >> name;
        value = parse(name);
        return stream;
    }

    const std::vector<TEnum>& getValues() {
        initialize();
        return values;
    }

protected:
    using member_data = std::vector<std::pair<TEnum, std::string>>;

    virtual std::string getTypeName() = 0;
    virtual member_data getMemberData() = 0;

private:
    static std::string to_lower_copy(const std::string& s) {
        std::string out(s);
        std::transform(out.begin(), out.end(), out.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return out;
    }

    void initialize() {
        if (initialized) return;

        typeName = getTypeName();
        for (const auto& pair : getMemberData()) {
            TEnum value = pair.first;
            std::string name = pair.second;
            lowerCaseNameToValueMap[to_lower_copy(name)] = value;
            valueToNameMap[value] = name;
            values.push_back(value);
        }
        initialized = true;
    }

    bool initialized;
    std::string typeName;
    std::map<std::string, TEnum> lowerCaseNameToValueMap;
    std::map<TEnum, std::string> valueToNameMap;
    std::vector<TEnum> values;
};
