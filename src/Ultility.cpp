#include "Ultility.h"


std::vector<std::string> splitString(const std::string &str, const std::string &token) {
    std::vector<std::string> res;
    std::string::size_type prev = 0;
    std::string::size_type pos = str.find(token, prev);
    while (pos != std::string::npos) {
        res.push_back(str.substr(prev, pos - prev));
        prev = pos+1;
        pos  = str.find(token, prev);
    }
    res.push_back(str.substr(prev));

    return res;
}
