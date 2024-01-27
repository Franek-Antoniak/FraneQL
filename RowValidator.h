#pragma once

#include "Table.h"
#include <optional>


class RowValidator {
public:
    static void validateDataInsertion(const Table& table, const Row& row);
};