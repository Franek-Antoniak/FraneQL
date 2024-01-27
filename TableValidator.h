#pragma once

#include "Table.h"
#include "memory"
#include "Column.h"
#include "Database.h"

class TableValidator {
public:
    static void validateTableCreation(const Table& table, const Database& database);

    static void validateColumnAddition(const Table& table, const std::shared_ptr<Column>& column);
};