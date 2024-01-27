#include "Query.h"

Condition::Condition(std::string column, std::string op, std::string value)
        : column(std::move(column)), op(std::move(op)), value(std::move(value)) {}

Condition::Condition(std::string column, std::string op) : column(std::move(column)), op(std::move(op)) {}

ConditionGroup::ConditionGroup(TokenType logicalOperator) : logicalOperator(logicalOperator) {}

void ConditionGroup::addCondition(const Condition &condition) {
    conditions.emplace_back(condition);
}

void ConditionGroup::addConditionGroup(const ConditionGroup &conditionGroup) {
    conditions.emplace_back(conditionGroup);
}

// Default to AND for top-level group
SelectQuery::SelectQuery() : whereClause(TokenType::AND) {}

void SelectQuery::addConditionGroup(const ConditionGroup &conditionGroup) {
    whereClause.addConditionGroup(conditionGroup);
}


// Data Transfer Objects
ParsedRelation::ParsedRelation(std::string foreignKeyColumnName, std::string referencedTableName,
                               std::string referencedColumnName)
        : foreignKeyColumnName(std::move(foreignKeyColumnName)),
          referencedTableName(std::move(referencedTableName)),
          referencedColumnName(std::move(referencedColumnName)) {}

const std::string &ParsedRelation::getForeignKeyColumnName() const {
    return foreignKeyColumnName;
}

const std::string &ParsedRelation::getReferencedTableName() const {
    return referencedTableName;
}

const std::string &ParsedRelation::getReferencedColumnName() const {
    return referencedColumnName;
}