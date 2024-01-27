#include "Column.h"

#include <sstream>

Column::Column(std::string name, DataType type, ColumnConstraint constraint) : name(std::move(name)), type(type),
                                                                               constraints() {
    constraints.push_back(constraint);
}

Column::Column(std::string name, DataType type) : name(std::move(name)), type(type), constraints() {}

Column::Column(std::string name, DataType type, std::vector<ColumnConstraint> constraints):
name(std::move(name)), type(type), constraints(std::move(constraints)) {}


std::string Column::getName() const {
    return name;
}

DataType Column::getDataType() const {
    return type;
}

std::vector<ColumnConstraint> Column::getConstraints() const {
    return constraints;
}

std::shared_ptr<Table> Column::getTable() const {
    return table;
}

void Column::setTable(const std::shared_ptr<Table> &table_arg) {
    Column::table = table_arg;
}

