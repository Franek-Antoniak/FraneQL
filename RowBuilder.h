#pragma once

#include <map>
#include <memory>
#include <string>
#include "Table.h"
#include <optional>

class BoxedValue; // Forward declaration
class Row; // Forward declaration

class RowBuilder {
protected:
    std::map<std::shared_ptr<Column>, BoxedValue> rowData;
public:
    virtual RowBuilder &set(const std::shared_ptr<Column> &column, const BoxedValue &value);
    [[nodiscard]] virtual Row build() const;
};