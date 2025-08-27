#include <algorithm>
#include <cctype>
#include <string>

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
