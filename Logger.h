#pragma once

#include <string>

class Logger {
public:
    static void error(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
};
