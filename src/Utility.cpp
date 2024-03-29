#include <sys/types.h>
#include <sys/stat.h>
#include <iomanip>
#include "Utility.h"


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


bool isRegularFile(const std::string &file) {
    struct stat fstat;
    return stat(file.c_str(), &fstat) == 0 && S_ISREG(fstat.st_mode);
}


std::ostream &logDateTime(std::ostream &stream) {
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);
    return stream << std::put_time(&tm, "%c %Z") << ": ";
}
