#pragma once

#include "Lexer.h"
#include "Query.h"
#include "Token.h"
#include "vector"
#include <memory>

class Parser {
public:
    explicit Parser(Lexer &lexer);

    std::unique_ptr<Query> parseQuery(); // Parses the entire query
private:
    Lexer &lexer;
    Token currentToken;

    void nextToken(); // Moves to the next token
    virtual std::unique_ptr<Query> parseSelect(); // Parses a SELECT query
    virtual std::unique_ptr<Query> parseInsert(); // Parses an INSERT query
    virtual std::unique_ptr<Query> parseCreate(); // Parses a CREATE query
    virtual std::unique_ptr<Query> parseAlter();
    virtual std::unique_ptr<Query> parseDrop();

    // Helper methods for error reporting and checking
    virtual void expect(const std::vector<TokenType>& expectedTypes) const; // Ensure the current token is of the
    // expected types
    static void error(const std::string &message); // Handle errors

    // Helper methods for parsing specific parts of the SQL query
    virtual void parseColumns(std::unique_ptr<SelectQuery> &query);
    virtual void parseFrom(std::unique_ptr<SelectQuery> &query);
    virtual void parseWhere(std::unique_ptr<SelectQuery> &query);

    // Helper methods for parsing WHERE clause
    virtual ConditionGroup parseOrExpression();
    virtual ConditionGroup parseAndExpression();
    virtual ConditionGroup parseExpression();
    virtual ConditionGroup parseCondition();

    // Helper methods for parsing INSERT query
    virtual std::string parseTableName();
    virtual std::vector<std::string> parseColumns();
    virtual std::vector<std::string> parseValues();

    // Helper methods for parsing CREATE query
    virtual std::pair<std::vector<Column>, std::vector<ParsedRelation>> parseColumnDefinitionsAndRelations();
    virtual bool isColumnConstraint(TokenType tokenType);
    virtual bool isTableConstraint(TokenType tokenType);
    virtual ColumnConstraint parseColumnConstraint();
    virtual TableConstraint parseTableConstraint();
    virtual ParsedRelation createRelation(const TableConstraint &constraint);

    // Helper methods for parsing ALTER query
    virtual Column parseColumnDefinition();
    virtual std::string parseColumnName();
};