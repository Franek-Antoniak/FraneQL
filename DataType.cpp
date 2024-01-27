#include "DataType.h"

#include <sstream>
#include <stdexcept>
#include <iomanip>

DataType DataTypeUtils::fromString(const std::string &type) {
    if (type == "FLOAT") {
        return DataType::FLOAT; // Return FLOAT for "FLOAT"
    } else if (type == "INTEGER") {
        return DataType::INTEGER; // Return INTEGER for "INTEGER"
    } else if (type == "TEXT") {
        return DataType::TEXT; // Return TEXT for "TEXT"
    } else if (type == "BOOLEAN") {
        return DataType::BOOLEAN; // Return BOOLEAN for "BOOLEAN"
    } else if (type == "DOUBLE") {
        return DataType::DOUBLE; // Return DOUBLE for "DOUBLE"
    } else if (type == "CHAR") {
        return DataType::CHAR; // Return CHAR for "CHAR"
    } else if (type == "DATE") {
        return DataType::DATE; // Return DATE for "DATE"
    } else if (type == "TIME") {
        return DataType::TIME; // Return TIME for "TIME"
    } else if (type == "DATETIME") {
        return DataType::DATETIME; // Return DATETIME for "DATETIME"
    } else {
        throw std::invalid_argument("Unknown data type: " + type); // Throw an exception for unknown data types
    }
}


Date::Date(const std::string &dateStr) {
    struct tm tm{};
    if (strptime(dateStr.c_str(), "%Y-%m-%d", &tm) == nullptr) {
        throw std::invalid_argument("Invalid date value");
    }
    year = tm.tm_year + 1900; // years since 1900
    month = tm.tm_mon + 1; // months since January [0-11]
    day = tm.tm_mday; // day of the month [1-31]
}

std::strong_ordering Date::operator<=>(const Date &other) const {
    if (auto cmp = year <=> other.year; cmp != 0) return cmp;
    if (auto cmp = month <=> other.month; cmp != 0) return cmp;
    return day <=> other.day;
}

bool Date::operator==(const Date &other) const {
    return year == other.year && month == other.month && day == other.day;
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << year << "-"
        << std::setw(2) << std::setfill('0') << month << "-"
        << std::setw(2) << std::setfill('0') << day;
    return oss.str();
}

Time::Time(const std::string &other) {
    struct tm tm{};

    if (strptime(other.c_str(), "%H:%M:%S", &tm) == nullptr) {
        throw std::invalid_argument("Invalid time value");
    }

    hour = tm.tm_hour;
    minute = tm.tm_min;
    second = tm.tm_sec;
}

std::strong_ordering Time::operator<=>(const Time &other) const {
    if (auto cmp = hour <=> other.hour; cmp != 0) return cmp;
    if (auto cmp = minute <=> other.minute; cmp != 0) return cmp;
    return second <=> other.second;
}

bool Time::operator==(const Time &other) const {
    return hour == other.hour && minute == other.minute && second == other.second;
}

std::string Time::toString() const {
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hour << ":"
        << std::setw(2) << std::setfill('0') << minute << ":"
        << std::setw(2) << std::setfill('0') << second;
    return oss.str();
}

DateTime::DateTime(const std::string &datetimeStr) {
    struct tm tm{};
    if (strptime(datetimeStr.c_str(), "%Y-%m-%dT%H:%M:%S", &tm) == nullptr) {
        throw std::invalid_argument("Invalid datetime value");
    }
    date = Date(datetimeStr.substr(0, 10)); // extract date part
    time = Time(datetimeStr.substr(11, 19)); // extract time part
}

std::strong_ordering DateTime::operator<=>(const DateTime &other) const {
    if (auto cmp = date <=> other.date; cmp != 0) return cmp;
    return time <=> other.time;
}

bool DateTime::operator==(const DateTime &other) const {
    return date == other.date && time == other.time;
}

std::string DateTime::toString() const {
    return date.toString() + "T" + time.toString();
}
