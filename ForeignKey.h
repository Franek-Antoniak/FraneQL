#pragma once

#include <memory>
#include "Table.h"
#include "Column.h"
#include "PrimaryKey.h"

class PrimaryKey; // Forward declaration


class ForeignKey {
    std::shared_ptr<Column> keyColumn;
    std::shared_ptr<PrimaryKey> referencePrimaryKey;
public:
    ForeignKey(std::shared_ptr<Column> keyColumn, std::shared_ptr<PrimaryKey> referencePrimaryKey);

    [[nodiscard]] virtual std::shared_ptr<Table> getReferencedTable() const;

    [[nodiscard]] virtual std::shared_ptr<Column> getKeyColumn() const;

    [[nodiscard]] virtual std::shared_ptr<PrimaryKey> getReferencePrimaryKey() const;
};
