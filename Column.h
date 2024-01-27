#pragma once

#include "DataType.h"
#include "Table.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>

class Table; // Forward declaration

enum class ColumnConstraint {
    NOT_NULL,
    UNIQUE,
    PRIMARY_KEY,
};

// Abstract class for a column
class Column {
    std::string name; // Private variable to store the name of the column
    DataType type; // Private variable to store the type of the column
    std::vector<ColumnConstraint> constraints; // Private variable to store the constraint of the column
    std::shared_ptr<Table> table; // Private variable to store the table of the column
public:
    Column(std::string name, DataType type, std::vector<ColumnConstraint> constraints);
    Column(std::string name, DataType type);
    Column(std::string name, DataType type, ColumnConstraint constraint);
    [[nodiscard]] virtual std::string getName() const; //  virtual function to get the name of the column
    [[nodiscard]] virtual DataType getDataType() const; //  virtual function to get the data type of the column
    [[nodiscard]] virtual std::vector<ColumnConstraint> getConstraints() const; //  virtual function to get the constraints of the column
    [[nodiscard]] virtual std::shared_ptr<Table> getTable() const; //  virtual function to get the table of the column
    virtual void setTable(const std::shared_ptr<Table> &table); //  virtual function to set the table of the column
};