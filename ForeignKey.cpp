#include "ForeignKey.h"

ForeignKey::ForeignKey(std::shared_ptr<Column> keyColumn, std::shared_ptr<PrimaryKey> referencePrimaryKey)
: keyColumn(std::move(keyColumn)), referencePrimaryKey(std::move(referencePrimaryKey)) {}

std::shared_ptr<Table> ForeignKey::getReferencedTable() const {
    return referencePrimaryKey->getTable();
}

std::shared_ptr<Column> ForeignKey::getKeyColumn() const {
    return keyColumn;
}

std::shared_ptr<PrimaryKey> ForeignKey::getReferencePrimaryKey() const {
    return referencePrimaryKey;
}
