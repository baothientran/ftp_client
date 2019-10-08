#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>

std::vector<std::string> splitString(const std::string &str, const std::string &token);

template<typename Iter>
std::string joinString(Iter begin, Iter end, const std::string &token) {
    std::string res;
    std::string sep = "";
    for (auto it = begin; it != end; ++it) {
        res += sep + *it;
        sep = token;
    }

    return res;
}

#endif
