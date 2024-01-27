#pragma once

#include <string>

// Enum class for data types
enum class DataType {
    INTEGER, // Represents integer data type
    TEXT, // Represents text data type
    BOOLEAN, // Represents boolean data type
    FLOAT, // Represents float data type
    DOUBLE, // Represents double data type
    CHAR, // Represents char data type
    DATE, // Represents date data type
    TIME, // Represents time data type
    DATETIME,   // Represents datetime data type
};

class DataTypeUtils {
public:
    // Helper function to convert a string to DataType
    static DataType fromString(const std::string &type);
};

struct Date {
    int year{};
    int month{};
    int day{};

    Date() = default;

    explicit Date(const std::string &other);

    virtual std::strong_ordering operator<=>(const Date &other) const;

    virtual bool operator==(const Date &other) const;

    [[nodiscard]] virtual std::string toString() const;
};

struct Time {
    int hour{};
    int minute{};
    int second{};

    Time() = default;

    explicit Time(const std::string &other);

    virtual std::strong_ordering operator<=>(const Time &other) const;

    virtual bool operator==(const Time &other) const;

    [[nodiscard]] virtual std::string toString() const;
};

struct DateTime {
    Date date{};
    Time time{};

    DateTime() = default;

    explicit DateTime(const std::string &datetimeStr);
    virtual std::strong_ordering operator<=>(const DateTime &other) const;
    virtual bool operator==(const DateTime &other) const;

    [[nodiscard]] virtual std::string toString() const;
};

