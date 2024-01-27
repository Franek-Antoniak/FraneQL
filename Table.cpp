#include "Table.h"

#include "TableValidator.h"
#include "RowValidator.h"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>

// TABLE
Table::Table(std::string name) : name(std::move(name)) {} // Constructor

void Table::addColumn(std::shared_ptr<Column> column) {
    TableValidator::validateColumnAddition(*this, column); // Validate the column addition


    column->setTable(shared_from_this()); // Set the column's table to this table

    columns.push_back(std::move(column)); // Add a column to the table

    column = columns.back(); // Get a reference to the newly added column

    // Add a null value for the new column in each existing row
    for (auto &row : rows) {
        row.data[column] = BoxedValue(column->getDataType(), std::nullopt);
    }
}

void Table::addRow(const RowBuilder &builder) {
    auto rowData = builder.build();
    Row newRow;

    // Validate each column data
    for (const auto &[column, value]: rowData) {
        newRow.data[column] = value; // Add the column data to the new row
    }

    // For any missing columns, add empty values
    for (const auto &column: columns) {
        if (newRow.data.find(column) == newRow.end()) {
            newRow.data[column] = BoxedValue(column->getDataType(), std::nullopt);
        }
    }

    RowValidator::validateDataInsertion(*this, newRow); // Validate the row addition

    rows.emplace_back(std::move(newRow)); // Add the new row to the table
}


void Table::setPrimaryKey(const PrimaryKey &primaryKeyArg) {
    this->primaryKey = std::make_shared<PrimaryKey>(primaryKeyArg);
}

void Table::addForeignKey(const ForeignKey &foreignKey) {
    foreignKeys.push_back(foreignKey);
}

void Table::addRelation(const Relation &relation) {
    relations.push_back(relation);
}


// getters that returns reference that cannot be modified
const std::string &Table::getName() const {
    return name; // Return the name of the table
}

const std::vector<std::shared_ptr<Column>> &Table::getColumns() const {
    return columns; // Return the vector of columns
}

const std::vector<Row> &Table::getRows() const {
    return rows; // Return the vector of rows
}

const std::shared_ptr<PrimaryKey> &Table::getPrimaryKey() const {
    return primaryKey;
}

std::optional<std::shared_ptr<Column>> Table::getColumn(const std::string &basicString) const {
    auto it = std::ranges::find_if(columns, [&](const auto &column) {
        return column->getName() == basicString;
    });
    if (it == columns.end()) {
        return std::nullopt;
    }
    return *it;
}

const std::vector<ForeignKey> &Table::getForeignKeys() const {
    return foreignKeys;
}

const std::vector<Relation> &Table::getRelations() const {
    return relations;
}

void Table::dropColumn(const std::string &columnName) {
    // Find the column to be dropped
    auto it = std::find_if(columns.begin(), columns.end(), [&](const auto &column) {
        return column->getName() == columnName;
    });

    // If the column is not found, throw an exception
    if (it == columns.end()) {
        throw std::runtime_error("Column " + columnName + " not found in table " + name);
    }

    // Before removing the column
    auto column = *it;

    // Remove the column from the columns vector
    columns.erase(it);

    // Remove the column from each row
    for (auto &row : rows) {
        row.data.erase(column);
    }

    // Remove all foreign keys that involve the column
    foreignKeys.erase(std::remove_if(foreignKeys.begin(), foreignKeys.end(), [&](const ForeignKey &foreignKey) {
        return foreignKey.getKeyColumn() == column;
    }), foreignKeys.end());

    // Remove all relations that involve the column
    relations.erase(std::remove_if(relations.begin(), relations.end(), [&](const Relation &relation) {
        return relation.getForeignKey()->getKeyColumn() == column;
    }), relations.end());
}

// ROW
std::map<std::shared_ptr<Column>, BoxedValue>::const_iterator Row::end() const {
    return data.end();
}

std::map<std::shared_ptr<Column>, BoxedValue>::const_iterator Row::begin() const {
    return data.begin();
}

Row::Row(std::map<std::shared_ptr<Column>, BoxedValue> map) : data(std::move(map)) {}

BoxedValue Row::operator[](const std::shared_ptr<Column> &column) const {
    return data.at(column);
}

BoxedValue Row::operator[](const std::string &columnName) const {
    auto it = std::ranges::find_if(data, [&](const auto &pair) {
        return pair.first->getName() == columnName;
    });
    if (it == data.end()) {
        throw std::runtime_error("Column not found");
    }
    return it->second;
}

std::strong_ordering BoxedValue::operator<=>(const BoxedValue &other) const {
    if (type != other.type) {
        throw std::runtime_error("Cannot compare values of different types");
    }

    if (!data.has_value() && !other.data.has_value()) {
        return std::strong_ordering::equal;
    } else if (!data.has_value()) {
        return std::strong_ordering::less;
    } else if (!other.data.has_value()) {
        return std::strong_ordering::greater;
    }

    auto thisValue = data.value();
    auto otherValue = other.data.value();

    // Compare the values based on the type
    switch (type) {
        case DataType::INTEGER:
            // Compare the int values from the variants
            return std::get<int>(thisValue) <=> std::get<int>(otherValue);
        case DataType::FLOAT: {
            float thisFloat = std::get<float>(thisValue);
            float otherFloat = std::get<float>(otherValue);

            if (std::isnan(thisFloat) || std::isnan(otherFloat)) {
                // Handle NaN values based on your application logic
                // For example, you can consider NaN values as greater or lesser than other values.
                // Here's an example of considering NaN as greater:
                if (std::isnan(thisFloat) && std::isnan(otherFloat))
                    return std::strong_ordering::equal;  // Both are NaN
                else if (std::isnan(thisFloat))
                    return std::strong_ordering::greater;  // This is NaN, other is not
                else
                    return std::strong_ordering::less;  // This is not NaN, other is NaN
            } else {
                // Use a custom comparison for floats that returns std::strong_ordering
                return (thisFloat < otherFloat) ? std::strong_ordering::less
                                                : (thisFloat > otherFloat) ? std::strong_ordering::greater
                                                                           : std::strong_ordering::equal;
            }
        }
        case DataType::BOOLEAN:
            // Compare the bool values from the variants
            return std::get<bool>(thisValue) <=> std::get<bool>(otherValue);
        case DataType::TEXT:
            // Compare the string values from the variants
            return std::get<std::string>(thisValue) <=> std::get<std::string>(otherValue);
        case DataType::DOUBLE: {
            double thisDouble = std::get<double>(thisValue);
            double otherDouble = std::get<double>(otherValue);

            if (std::isnan(thisDouble) || std::isnan(otherDouble)) {
                // Handle NaN values based on your application logic
                // For example, you can consider NaN values as greater or lesser than other values.
                // Here's an example of considering NaN as greater:
                if (std::isnan(thisDouble) && std::isnan(otherDouble))
                    return std::strong_ordering::equal;  // Both are NaN
                else if (std::isnan(thisDouble))
                    return std::strong_ordering::greater;  // This is NaN, other is not
                else
                    return std::strong_ordering::less;  // This is not NaN, other is NaN
            } else {
                // Use a custom comparison for doubles that returns std::strong_ordering
                return (thisDouble < otherDouble) ? std::strong_ordering::less
                                                  : (thisDouble > otherDouble) ? std::strong_ordering::greater
                                                                               : std::strong_ordering::equal;
            }
        }
        case DataType::CHAR:
            // Compare the char values from the variants
            return std::get<char>(thisValue) <=> std::get<char>(otherValue);
        case DataType::DATE:
            // Compare the Date values from the variants
            return std::get<Date>(thisValue) <=> std::get<Date>(otherValue);
        case DataType::TIME:
            // Compare the Time values from the variants
            return std::get<Time>(thisValue) <=> std::get<Time>(otherValue);
        case DataType::DATETIME:
            // Compare the DateTime values from the variants
            return std::get<DateTime>(thisValue) <=> std::get<DateTime>(otherValue);
        default:
            throw std::runtime_error("Unsupported type");
    }
    // Unreachable
}


bool BoxedValue::operator==(const BoxedValue &other) const {
    // If both data members are null, they are considered equal
    if (!data.has_value() && !other.data.has_value()) {
        return true;
    }
    // If only one of the data members null, they are not equal
    if (!data.has_value() || !other.data.has_value()) {
        return false;
    }
    // If both data members have a value, compare the values
    // Note: You need to define how to compare VariantType values
    return data.value() == other.data.value();
}

std::string BoxedValue::toString() const {
    if (!data.has_value()) {
        return "NULL";
    }

    auto value = data.value();

    switch (type) {
        case DataType::INTEGER:
            return std::to_string(std::get<int>(value));
        case DataType::FLOAT:
            return std::to_string(std::get<float>(value));
        case DataType::BOOLEAN:
            return std::get<bool>(value) ? "true" : "false";
        case DataType::TEXT:
            return std::get<std::string>(value);
        case DataType::DOUBLE:
            return std::to_string(std::get<double>(value));
        case DataType::CHAR:
            return {std::get<char>(value)};
        case DataType::DATE:
            return std::get<Date>(value).toString();
        case DataType::TIME:
            return std::get<Time>(value).toString();
        case DataType::DATETIME:
            return std::get<DateTime>(value).toString();
        default:
            throw std::runtime_error("Unsupported type");
    }
    // Unreachable
}

bool BoxedValue::has_value() const {
    return data.has_value();
}

BoxedValue::BoxedValue(DataType type, std::optional<VariantType> data) : type(type), data(std::move(data)) {}

BoxedValue BoxedValue::fromString(const std::string &value, DataType type) {
    if (value == "NULL") {
        return {type, std::nullopt};
    }
    BoxedValue boxedValue;
    boxedValue.type = type;
    switch (type) {
        case DataType::INTEGER:
            boxedValue.data = std::stoi(value);
            break;
        case DataType::FLOAT:
            boxedValue.data = std::stof(value);
            break;
        case DataType::BOOLEAN:
            if (value == "true") {
                boxedValue.data = true;
            } else if (value == "false") {
                boxedValue.data = false;
            } else {
                throw std::invalid_argument("Invalid boolean value");
            }
            break;
        case DataType::TEXT:
            boxedValue.data = value;
            break;
        case DataType::DOUBLE:
            boxedValue.data = std::stod(value);
            break;
        case DataType::CHAR:
            if (value.length() != 1) {
                throw std::invalid_argument("Invalid char value");
            }
            boxedValue.data = value[0];
            break;
        case DataType::DATE: {
            boxedValue.data = Date(value);
            break;
        }
        case DataType::TIME: {
            boxedValue.data = Time(value);
            break;
        }
        case DataType::DATETIME: {
            boxedValue.data = DateTime(value);
            break;
        }
        default:
            throw std::invalid_argument("Unknown data type");
    }
    return boxedValue;
}
