#include "Database.h"
#include "fmt/core.h"
#include <algorithm>
#include "TableValidator.h"


void Database::createTable(const CreateTableQuery &query) {
    // Create a new table with the name and columns from the query
    auto table = std::make_shared<Table>(query.tableName);

    // Add the columns to the table
    for (const auto &column: query.columns) {
        table->addColumn(std::make_shared<Column>(column));
    }

    // Set the primary key of the table
    std::vector<std::shared_ptr<Column>> primaryKeyColumns;

    for (const auto &column: table->getColumns()) {
        for (const auto &constraint: column->getConstraints()) {
            if (constraint == ColumnConstraint::PRIMARY_KEY) {
                primaryKeyColumns.push_back(column);
                break;
            }
        }
    }

    // Check if there is only one primary key column
    if (primaryKeyColumns.size() > 1 || primaryKeyColumns.empty()) {
        throw std::runtime_error("More than one primary key column or no primary key column found");
    } else if (primaryKeyColumns.size() == 1) {
        table->setPrimaryKey(PrimaryKey(primaryKeyColumns.back()));
    }


    // Add the relations to the table
    for (const auto &relation: query.relations) {
        auto foreignKeyColumn = table->getColumn(relation.getForeignKeyColumnName());
        if (!foreignKeyColumn.has_value()) {
            throw std::runtime_error(
                    "Foreign key column " + relation.getForeignKeyColumnName() + " not found in table " +
                    query.tableName);
        }
        auto referencedTable = tables[relation.getReferencedTableName()];
        if (!referencedTable) {
            throw std::runtime_error("Referenced table " + relation.getReferencedTableName() + " not found");
        }
        auto referencedColumnName = referencedTable->getColumn(relation.getReferencedColumnName());
        if (!referencedColumnName.has_value()) {
            throw std::runtime_error(
                    "Referenced column " + relation.getReferencedColumnName() + " not found in table " +
                    relation.getReferencedTableName());
        }
        ForeignKey foreignKey(foreignKeyColumn.value(), std::make_shared<PrimaryKey>(referencedColumnName.value()));
        table->addForeignKey(foreignKey);
        Relation tableRelation(std::make_shared<ForeignKey>(foreignKey), referencedTable);
        table->addRelation(tableRelation);
    }

    TableValidator::validateTableCreation(*table, *this);

    // Add the table to the database
    tables[query.tableName] = table;
}

void Database::insertInto(const InsertQuery &query) {
    // Find the table
    std::vector<std::string> queryColumns = query.columns;

    auto it = tables.find(query.tableName);
    if (it == tables.end()) {
        throw std::runtime_error("Table with name " + query.tableName + " not found");
    }

    // Get the table
    auto table = it->second;

    // Create a RowBuilder
    RowBuilder builder;

    // Assuming tableColumns and values are in the same order
    auto tableColumns = table->getColumns();

    if (queryColumns.empty()) {
        // If no tableColumns are specified, we assume the order of the values matches the order of the tableColumns
        if (query.values.size() != tableColumns.size()) {
            throw std::runtime_error("Number of values does not match number of tableColumns");
        }

        std::ranges::transform(tableColumns, queryColumns.begin(), [](const auto &column) {
            return column->getName();
        });
    } else {
        // If tableColumns are specified, we assume the order of the values matches the order of the tableColumns
        if (query.values.size() != queryColumns.size()) {
            throw std::runtime_error("Number of values does not match number of tableColumns");
        }
    }

    for (size_t i = 0; i < queryColumns.size(); ++i) {
        auto lambda = [&](const auto &column) {
            return column->getName() == queryColumns[i];
        };
        auto found = std::find_if(tableColumns.begin(), tableColumns.end(), lambda);
        if (found == tableColumns.end()) {
            throw std::runtime_error("Column names do not match");
        }
        BoxedValue value;
        if (query.values[i] == "NULL") {
            value = BoxedValue((*found)->getDataType(), std::nullopt);
        } else {
            value = BoxedValue::fromString(query.values[i], (*found)->getDataType());
        }
        builder.set(*found, value);
    }

    // Insert the data into the table
    table->addRow(builder);
}

void Database::selectFrom(const SelectQuery &query) {
    // Find the table
    auto it = tables.find(query.fromTable);
    if (it == tables.end()) {
        throw std::runtime_error("Table not found");
    }

    // Columns to select
    auto columnsToProcess = query.columns;

    // Get the table
    auto table = it->second;

    // Get all rows from the table
    auto allRows = table->getRows();

    // If * is specified, replace it with all column names
    if (columnsToProcess.size() == 1 && columnsToProcess.back() == "*") {
        columnsToProcess.clear();
        columnsToProcess.resize(table->getColumns().size());
        std::ranges::transform(table->getColumns(), columnsToProcess.begin(), [](const auto &column) {
            return column->getName();
        });
    }


    // Container for rows that satisfy the conditions
    std::vector<Row> selectedRows;

    // Iterate over each row
    for (const auto &row: allRows) {
        // Check if the row satisfies the conditions
        if (satisfiesConditions(row, query.whereClause)) {
            // If it does, add it to selectedRows
            selectedRows.push_back(row);
        }
    }

    // Display the data in selectedRows
    columnsToDisplay(selectedRows, columnsToProcess);
}

bool Database::satisfiesConditions(const Row &row, const ConditionGroup &conditionGroup) {
    if (conditionGroup.logicalOperator == TokenType::AND) {
        // For AND conditions, use all_of
        return std::ranges::all_of(conditionGroup.conditions, [&](const auto &conditionVariant) {
            if (std::holds_alternative<Condition>(conditionVariant)) {
                const auto &condition = std::get<Condition>(conditionVariant);
                return satisfiesCondition(row, condition);
            } else {
                const auto &nestedConditionGroup = std::get<ConditionGroup>(conditionVariant);
                return satisfiesConditions(row, nestedConditionGroup);
            }
        });
    } else {
        // For OR conditions, use any_of
        return std::ranges::any_of(conditionGroup.conditions, [&](const auto &conditionVariant) {
            if (std::holds_alternative<Condition>(conditionVariant)) {
                const auto &condition = std::get<Condition>(conditionVariant);
                return satisfiesCondition(row, condition);
            } else {
                const auto &nestedConditionGroup = std::get<ConditionGroup>(conditionVariant);
                return satisfiesConditions(row, nestedConditionGroup);
            }
        });
    }
}

bool Database::satisfiesCondition(const Row &row, const Condition &condition) {
    // Find the column in the row
    auto it = std::find_if(row.begin(), row.end(), [&](const auto &pair) {
        return pair.first->getName() == condition.column;
    });

    // If the column is not found, return false
    if (it == row.end()) {
        return false;
    }
    if (condition.op == "IS_NULL") {
        return !(it->second.has_value());
    } else if (condition.op == "IS_NOT_NULL") {
        return it->second.has_value();
    } else {
        // Check if the data in the row satisfies the condition
        const auto condition_value = BoxedValue::fromString(condition.value, it->second.type);
        if (condition.op == "<=") {
            return it->second <= condition_value;
        } else if (condition.op == ">=") {
            return it->second >= condition_value;
        } else if (condition.op == "<>") {
            return it->second != condition_value;
        } else if (condition.op == "=") {
            return it->second == condition_value;
        } else if (condition.op == "<") {
            return it->second < condition_value;
        } else if (condition.op == ">") {
            return it->second > condition_value;
        } else {
            // If the operator is not supported, throw an exception
            throw std::runtime_error("Unsupported operator: " + condition.op);
        }
    }
}

void Database::columnsToDisplay(const std::vector<Row> &rows, const std::vector<std::string> &columnsToDisplay) {
    if (rows.empty()) {
        fmt::print("No data to display.\n");
        return;
    }

    // Structure to hold column name and its width
    struct ColumnInfo {
        std::string name;
        size_t width;
    };

    std::vector<ColumnInfo> columns;
    for (const auto &columnName: columnsToDisplay) {
        columns.push_back({columnName, columnName.length()});
    }

    // Update widths based on data
    for (const auto &row: rows) {
        for (auto &column: columns) {
            auto it = std::find_if(row.begin(), row.end(), [&](const auto &pair) {
                return pair.first->getName() == column.name;
            });
            if (it != row.end()) {
                column.width = std::max(column.width, it->second.toString().length());
            }
        }
    }

    // Create a horizontal separator
    std::string separator;
    for (const auto &column: columns) {
        separator += "+" + std::string(column.width + 2, '-');  // +2 for spacing
    }
    separator += "+\n";

    // Print header
    fmt::print("{}", separator);
    for (const auto &column: columns) {
        fmt::print("| {:^{}} ", column.name, column.width);
    }
    fmt::print("|\n{}", separator);

    // Print rows
    for (const auto &row: rows) {
        for (const auto &column: columns) {
            auto it = std::find_if(row.begin(), row.end(), [&](const auto &pair) {
                return pair.first->getName() == column.name;
            });
            fmt::print("| {:^{}} ", it != row.end() ? it->second.toString() : "[Data not found]", column.width);
        }
        fmt::print("|\n{}", separator);
    }
}

std::optional<std::shared_ptr<Table>> Database::getTableDefinition(const std::string &basicString) const {
    auto it = tables.find(basicString);
    if (it == tables.end()) {
        return std::nullopt;
    }
    return it->second;
}

void Database::alterTable(const AlterTableQuery &query) {
    // Find the table
    auto it = tables.find(query.tableName);
    if (it == tables.end()) {
        throw std::runtime_error("Table with name " + query.tableName + " not found");
    }

    // Get the table
    auto table = it->second;

    // Iterate over each operation in the query
    for (const auto &operation : query.operations) {
        if (std::holds_alternative<AddColumnOperation>(operation)) {
            // If it's an AddColumnOperation, add the column to the table
            const auto &addColumnOperation = std::get<AddColumnOperation>(operation);
            if (table->getColumn(addColumnOperation.column.getName()).has_value()) {
                throw std::runtime_error("Column with name " + addColumnOperation.column.getName() + " already exists");
            }

            // if there is primary key
            bool hasPrimaryKey = false;
            for (const auto &constraint: addColumnOperation.column.getConstraints()) {
                if (constraint == ColumnConstraint::PRIMARY_KEY) {
                    hasPrimaryKey = true;
                    break;
                }
            }
            if (hasPrimaryKey) {
                throw std::runtime_error("Cannot add primary key column");
            }

            for (const auto &constraint: addColumnOperation.column.getConstraints()) {
                if (constraint == ColumnConstraint::NOT_NULL) {
                    throw std::runtime_error("Cannot add NOT_NULL column");
                }
            }

            table->addColumn(std::make_shared<Column>(addColumnOperation.column));


        } else if (std::holds_alternative<DropColumnOperation>(operation)) {
            // If it's a DropColumnOperation, drop the column from the table
            const auto &dropColumnOperation = std::get<DropColumnOperation>(operation);
            if (!table->getColumn(dropColumnOperation.columnName).has_value()) {
                throw std::runtime_error("Column with name " + dropColumnOperation.columnName + " not found");
            }

            if (table->getPrimaryKey()->getKeyColumn()->getName() == dropColumnOperation.columnName) {
                throw std::runtime_error("Cannot drop primary key column");
            }

            table->dropColumn(dropColumnOperation.columnName);
        } else if (std::holds_alternative<AddForeignKeyOperation>(operation)) {
            // If it's an AddForeignKeyOperation, add the foreign key to the table
            const auto &addForeignKeyOperation = std::get<AddForeignKeyOperation>(operation);
            auto foreignKeyColumn = table->getColumn(addForeignKeyOperation.relation.getForeignKeyColumnName());
            if (!foreignKeyColumn.has_value()) {
                throw std::runtime_error(
                        "Foreign key column " + addForeignKeyOperation.relation.getForeignKeyColumnName() +
                        " not found in table " +query.tableName);
            }

            // Check if the column in the current table is not already a foreign key
            for (const auto &foreignKey : table->getForeignKeys()) {
                if (foreignKey.getKeyColumn()->getName() == foreignKeyColumn.value()->getName()) {
                    throw std::runtime_error(
                            "Column " + foreignKeyColumn.value()->getName() + " is already a foreign key in table " + query.tableName);
                }
            }

            auto referencedTable = tables[addForeignKeyOperation.relation.getReferencedTableName()];
            if (!referencedTable) {
                throw std::runtime_error("Referenced table " + addForeignKeyOperation.relation.getReferencedTableName() + " not found");

            }
            auto referencedColumnName = referencedTable->getColumn(addForeignKeyOperation.relation.getReferencedColumnName());
            if (!referencedColumnName.has_value()) {
                throw std::runtime_error(
                        "Referenced column " + addForeignKeyOperation.relation.getReferencedColumnName() +
                        " not found in table " +addForeignKeyOperation.relation.getReferencedTableName());
            }

            // Check if the column in the referenced table is a primary key or a unique key
            bool isPrimaryKeyOrUnique = false;
            for (const auto &constraint : referencedColumnName.value()->getConstraints()) {
                if (constraint == ColumnConstraint::PRIMARY_KEY || constraint == ColumnConstraint::UNIQUE) {
                    isPrimaryKeyOrUnique = true;
                    break;
                }
            }

            if (!isPrimaryKeyOrUnique) {
                throw std::runtime_error(
                        "Referenced column " + referencedColumnName.value()->getName() + " in table " +
                        addForeignKeyOperation.relation.getReferencedTableName() + " is not a primary key or a unique key");
            }

            ForeignKey foreignKey(foreignKeyColumn.value(), std::make_shared<PrimaryKey>(referencedColumnName.value()));
            table->addForeignKey(foreignKey);
            Relation tableRelation(std::make_shared<ForeignKey>(foreignKey), referencedTable);
            table->addRelation(tableRelation);
        } else {
            // If the operation is not supported, throw an exception
            throw std::runtime_error("Unsupported operation");
        }
    }
}


void Database::dropTable(const DropTableQuery &query) {
    // Find the table
    auto it = tables.find(query.tableName);
    if (it == tables.end()) {
        // If the table is not found, throw an exception
        throw std::runtime_error("Table " + query.tableName + " not found");
    }

    // Remove the table from the map
    tables.erase(it);
}