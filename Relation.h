#pragma once

#include <memory>
#include "Column.h"
#include "ForeignKey.h"
#include "Table.h"

class ForeignKey; // Forward declaration


// New class Relation
class Relation {
    std::shared_ptr<ForeignKey> foreignKey;
    std::shared_ptr<Table> referencedTable;
public:
    Relation(std::shared_ptr<ForeignKey> foreignKey, std::shared_ptr<Table> referencedTable);
    // Add getters here

    [[nodiscard]] virtual std::shared_ptr<ForeignKey> getForeignKey() const;
    [[nodiscard]] virtual std::shared_ptr<Table> getReferencedTable() const;
};
