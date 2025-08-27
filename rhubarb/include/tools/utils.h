#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>

// trim left in-place
inline void ltrim(std::string &s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                         [](unsigned char ch){ return !std::isspace(ch); }));
}

// trim right in-place
inline void rtrim(std::string &s) {
    s.erase(
        std::find_if(s.rbegin(), s.rend(),
                     [](unsigned char ch){ return !std::isspace(ch); }).base(),
        s.end());
}

// trim both ends in-place
inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// replacement for boost::lexical_cast<std::string>
template <typename T>
inline std::string lexical_cast(const T& value)
{
    std::ostringstream oss;
	oss << value;
    return oss.str();
}

inline void trim_char(std::string& s, char ch) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [ch](char c){ return c != ch; }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [ch](char c){ return c != ch; }).base(), s.end());
}

