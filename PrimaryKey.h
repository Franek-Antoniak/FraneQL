#pragma once

#include "Table.h"
#include "Column.h"

class Column; // Forward declaration
class Table; // Forward declaration


// New class PrimaryKey
class PrimaryKey {
    std::shared_ptr<Column> keyColumn;
public:
    explicit PrimaryKey(std::shared_ptr<Column> keyColumn);

    [[nodiscard]] virtual std::shared_ptr<Table> getTable() const;
    [[nodiscard]] virtual std::shared_ptr<Column> getKeyColumn() const;
};