#pragma once

#include "Table.h"
#include <memory>
#include "Query.h"


class Database {
    std::map<std::string, std::shared_ptr<Table>> tables;
    virtual bool satisfiesCondition(const Row &row, const Condition &condition);
    virtual bool satisfiesConditions(const Row &row, const ConditionGroup &conditionGroup);
    virtual void columnsToDisplay(const std::vector<Row>& rows, const std::vector<std::string>& columnsToDisplay);
public:
    Database() = default; // Default constructor
    virtual void createTable(const CreateTableQuery &query);
    virtual void insertInto(const InsertQuery &query);
    virtual void selectFrom(const SelectQuery &query);
    virtual void alterTable(const AlterTableQuery &query);
    virtual void dropTable(const DropTableQuery &query);
    [[nodiscard]] virtual std::optional<std::shared_ptr<Table>>  getTableDefinition(const std::string &basicString) const;
};