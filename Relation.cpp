#include "Relation.h"

Relation::Relation(std::shared_ptr<ForeignKey> foreignKey, std::shared_ptr<Table> referencedTable)
        : foreignKey(std::move(foreignKey)), referencedTable(std::move(referencedTable)) {}

std::shared_ptr<ForeignKey> Relation::getForeignKey() const {
    return foreignKey;
}

std::shared_ptr<Table> Relation::getReferencedTable() const {
    return referencedTable;
}
