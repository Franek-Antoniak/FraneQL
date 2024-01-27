#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Column.h"
#include "ForeignKey.h"
#include "PrimaryKey.h"
#include "Relation.h"
#include "RowBuilder.h"
#include <optional>
#include <variant>

class RowBuilder; // Forward declaration
class Relation; // Forward declaration

enum class TableConstraint {
    FOREIGN_KEY
};

using VariantType = std::variant<int, float, bool, double, char, Date, DateTime, Time, std::string>;

struct BoxedValue {
    DataType type{};
    std::optional<VariantType> data;

    BoxedValue() = default;

    BoxedValue(DataType type, std::optional<VariantType> data);

    virtual std::strong_ordering operator<=>(const BoxedValue &other) const;

    virtual bool operator==(const BoxedValue &other) const;

    [[nodiscard]] virtual std::string toString() const;

    [[nodiscard]] virtual bool has_value() const;

    static BoxedValue fromString(const std::string &value, DataType type);
};

struct Row {
    explicit Row(std::map<std::shared_ptr<Column>, BoxedValue> map);

    Row() = default;

    std::map<std::shared_ptr<Column>, BoxedValue> data{};

    [[nodiscard]] virtual std::map<std::shared_ptr<Column>, BoxedValue>::const_iterator begin() const;

    [[nodiscard]] virtual std::map<std::shared_ptr<Column>, BoxedValue>::const_iterator end() const;

    [[nodiscard]] virtual BoxedValue operator[](const std::shared_ptr<Column> &column) const;

    [[nodiscard]] virtual BoxedValue operator[](const std::string &columnName) const;
};

class Table : public std::enable_shared_from_this<Table> {
    std::string name; // Name of the table
    std::vector<std::shared_ptr<Column>> columns; // List of pointers to columns in the table
    std::vector<Row> rows; // Each row is a map with column pointers as keys
    std::shared_ptr<PrimaryKey> primaryKey; // New member variable
    std::vector<ForeignKey> foreignKeys; // New member variable
    std::vector<Relation> relations; // New member variable
public:
    explicit Table(std::string name); // Constructor
    virtual void addColumn(std::shared_ptr<Column> column); //  virtual function to add a column to the table
    virtual void addRow(const RowBuilder &builder); //  virtual function to load data into the table
    virtual void dropColumn(const std::string &columnName); //  virtual function to drop a column from the table

    virtual void setPrimaryKey(const PrimaryKey &primaryKeyArg);

    virtual void addForeignKey(const ForeignKey &foreignKey);

    virtual void addRelation(const Relation &relation);

    // getters that returns reference that cannot be modified
    [[nodiscard]] virtual const std::string &getName() const;

    [[nodiscard]] virtual const std::vector<std::shared_ptr<Column>> &getColumns() const;

    [[nodiscard]] virtual const std::vector<Row> &getRows() const;

    [[nodiscard]] virtual const std::shared_ptr<PrimaryKey> &getPrimaryKey() const;

    [[nodiscard]] virtual const std::vector<ForeignKey> &getForeignKeys() const;

    [[nodiscard]] virtual const std::vector<Relation> &getRelations() const;

    [[nodiscard]] virtual std::optional<std::shared_ptr<Column>> getColumn(const std::string &basicString) const;
};

