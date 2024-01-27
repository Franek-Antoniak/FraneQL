#include "RowValidator.h"

#include <algorithm>



void RowValidator::validateDataInsertion(const Table& table, const Row& row) {
    // Check column constraints
    for (const auto& column : table.getColumns()) {
        const auto& value = row.data.at(column);
        std::vector<ColumnConstraint> constraints = column->getConstraints();
        if (std::ranges::find(constraints, ColumnConstraint::NOT_NULL) != constraints.end()
            || std::ranges::find(constraints, ColumnConstraint::PRIMARY_KEY) != constraints.end()) {
            if (!value.has_value()) {
                throw std::runtime_error("Value for column " + column->getName() + " cannot be null");
            }
        }

        if (std::ranges::find(constraints, ColumnConstraint::UNIQUE) != constraints.end()
            || std::ranges::find(constraints, ColumnConstraint::PRIMARY_KEY) != constraints.end()) {
            for (const auto& existingRow : table.getRows()) {
                if (value.has_value() && existingRow.data.at(column) == value) {
                    throw std::runtime_error("Value " + value.toString() + " already exists for column " + column->getName());
                }
            }
        }
    }

    for (const auto& foreignKey : table.getForeignKeys()) {
        const auto& foreignKeyColumn = foreignKey.getKeyColumn();
        const auto& referencedColumn = foreignKey.getReferencePrimaryKey()->getKeyColumn();
        const auto& referencedTable = foreignKey.getReferencedTable();
        const auto& value = row.data.at(foreignKeyColumn);
        if (value.has_value()) {
            bool found = false;
            for (const auto& existingRow : referencedTable->getRows()) {
                if (existingRow.data.at(referencedColumn) == value) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw std::runtime_error("Value " + value.toString() + " does not exist in referenced table " +
                referencedTable->getName());
            }
        }
    }
}