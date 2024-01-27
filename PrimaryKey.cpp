#include "PrimaryKey.h"

std::shared_ptr<Column> PrimaryKey::getKeyColumn() const {
    return keyColumn;
}

PrimaryKey::PrimaryKey(std::shared_ptr<Column> keyColumn) : keyColumn(std::move(keyColumn)) {}

std::shared_ptr<Table> PrimaryKey::getTable() const {
    return keyColumn->getTable();
}
