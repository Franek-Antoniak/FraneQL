#include "TableValidator.h"
#include <set>
#include <algorithm>

void TableValidator::validateTableCreation(const Table &table, const Database &database) {
    // check if table has a primary key
    if (!table.getPrimaryKey()) {
        throw std::runtime_error("Table must have a primary key");
    }

    // check if there is a table with the same name
    if (database.getTableDefinition(table.getName()).has_value()) {
        throw std::runtime_error("Table with name " + table.getName() + " already exists");
    }

    // check if column names are unique
    auto listOfColumnNames = std::vector<std::string>(table.getColumns().size());
    std::transform(table.getColumns().begin(), table.getColumns().end(), listOfColumnNames.begin(),
                   [](const auto &column) { return column->getName(); });
    auto setOfColumnNames = std::set<std::string>(listOfColumnNames.begin(), listOfColumnNames.end());

    if (setOfColumnNames.size() != listOfColumnNames.size()) {
        throw std::runtime_error("Column names must be unique");
    }
}

void TableValidator::validateColumnAddition(const Table &table, const std::shared_ptr<Column> &column) {
    // check if there is a column with the same name
    if (table.getColumn(column->getName()).has_value()) {
        throw std::runtime_error("Column with name " + column->getName() + " already exists");
    }
}
