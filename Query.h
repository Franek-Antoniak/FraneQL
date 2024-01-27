#pragma once


#include "Token.h"
#include "Column.h"
#include "Query.h"

#include <string>
#include <vector>
#include <memory>
#include <variant>


// Abstract base class for all query types
class Query {
public:
    virtual ~Query() = default;
};

class ParsedRelation {
    std::string foreignKeyColumnName;
    std::string referencedTableName;
    std::string referencedColumnName;
public:
    ParsedRelation(std::string foreignKeyColumnName, std::string referencedTableName, std::string referencedColumnName);

    [[nodiscard]] virtual const std::string &getForeignKeyColumnName() const;

    [[nodiscard]] virtual const std::string &getReferencedTableName() const;

    [[nodiscard]] virtual const std::string &getReferencedColumnName() const;
};

// Helper class for ALTER TABLE queries
class AddColumnOperation {
public:
    Column column; // The column to be added

    explicit AddColumnOperation(Column column)
            : column(std::move(column)) {}
};

// Helper class for ALTER TABLE queries
class DropColumnOperation {
public:
    std::string columnName; // The name of the column to be dropped

    explicit DropColumnOperation(std::string columnName)
            : columnName(std::move(columnName)) {}
};

// Helper class for ALTER TABLE queries
class AddForeignKeyOperation {
public:
    ParsedRelation relation; // The relation to be added

    explicit AddForeignKeyOperation(ParsedRelation relation)
            : relation(std::move(relation)) {}
};

// Represents an ALTER TABLE query
class AlterTableQuery : public Query {
public:
    std::string tableName; // The name of the table to be altered
    std::vector<std::variant<AddColumnOperation, DropColumnOperation, AddForeignKeyOperation>> operations; // The operations to be performed

    AlterTableQuery(std::string tableName,
                    std::vector<std::variant<AddColumnOperation, DropColumnOperation, AddForeignKeyOperation>> operations)
            : tableName(std::move(tableName)), operations(std::move(operations)) {}
};

// Represents a DROP TABLE query
class DropTableQuery : public Query {
public:
    std::string tableName; // The name of the table to be dropped

    explicit DropTableQuery(std::string tableName)
            : tableName(std::move(tableName)) {}
};

// Represents an INSERT query
class InsertQuery : public Query {
public:
    std::string tableName;            // Into which table
    std::vector<std::string> columns; // Optional list of columns
    std::vector<std::string> values;  // Values to insert
};

// Represents a CREATE TABLE query
class CreateTableQuery : public Query {
public:
    std::string tableName;            // Name of the table to create
    std::vector<Column> columns; // List of columns and their types
    std::vector<ParsedRelation> relations; // List of relations
};

// Represents a condition in the WHERE clause
class Condition {
public:
    std::string column; // The column name of the condition
    std::string value;  // The data to compare against
    std::string op;     // The operator (e.g., =, <, >, etc.)

    Condition(std::string column, std::string op);

    Condition(std::string column, std::string op, std::string value);
};

// Represents a Condition with parentheses
class ConditionGroup {
public:
    std::vector<std::variant<Condition, ConditionGroup>> conditions;
    TokenType logicalOperator; // AND or OR

    explicit ConditionGroup(TokenType logicalOperator);

    virtual void addCondition(const Condition &condition);

    virtual void addConditionGroup(const ConditionGroup &conditionGroup);
};

class SelectQuery : public Query {
public:
    std::vector<std::string> columns;   // List of columns to select
    std::string fromTable;              // From which table
    ConditionGroup whereClause;         // Conditions in the WHERE clause

    SelectQuery();

    virtual void addConditionGroup(const ConditionGroup &conditionGroup);
};
