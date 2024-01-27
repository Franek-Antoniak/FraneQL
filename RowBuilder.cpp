#include "RowBuilder.h"


RowBuilder &RowBuilder::set(const std::shared_ptr<Column> &column, const BoxedValue &value) {
    rowData[column] = value;
    return *this; // Return a reference to this object to allow chaining
}

Row RowBuilder::build() const {
    return Row(rowData);
}


