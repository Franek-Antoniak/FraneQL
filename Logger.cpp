#include "Logger.h"

#include "fmt/color.h"
#include "chrono"
#include <ctime>

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    return buf;
}

void Logger::error(const std::string& message) {
    std::string time = getCurrentTime();
    fmt::print(fmt::fg(fmt::color::red), "{} ERROR --- {}\n", time, message);
}

void Logger::info(const std::string &message) {
    std::string time = getCurrentTime();
    fmt::print(fmt::fg(fmt::color::green), "{} INFO --- {}\n", time, message);
}

void Logger::warning(const std::string &message) {
    std::string time = getCurrentTime();
    fmt::print(fmt::fg(fmt::color::yellow), "{} WARNING --- {}\n", time, message);
}