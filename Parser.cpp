#include "Parser.h"

#include <algorithm>

Parser::Parser(Lexer &lexer) : lexer(lexer) {
    // Initialize by pulling the first token
    nextToken();
}

// Can throw errors -> we have to catch them later
std::unique_ptr<Query> Parser::parseQuery() {
    // Based on the current token, decide which type of query to parse
    if (currentToken.type == TokenType::SELECT) {
        nextToken(); // Consumes SELECT
        return parseSelect();
    } else if (currentToken.type == TokenType::INSERT) {
        nextToken(); // Consumes INSERT
        return parseInsert();
    } else if (currentToken.type == TokenType::CREATE) {
        nextToken(); // Consumes CREATE
        return parseCreate();
    } else if (currentToken.type == TokenType::ALTER) {
        nextToken(); // Consumes ALTER
        return parseAlter();
    } else if (currentToken.type == TokenType::DROP) {
        nextToken(); // Consumes DROP
        return parseDrop();
    } else {
        // Handle other types or error
        expect({TokenType::SELECT, TokenType::INSERT, TokenType::CREATE, TokenType::ALTER, TokenType::DROP});
    }
}

void Parser::nextToken() {
    currentToken = lexer.nextToken();
}

void Parser::expect(const std::vector<TokenType> &expectedTypes) const {
    if (std::find(expectedTypes.begin(), expectedTypes.end(), currentToken.type) == expectedTypes.end()) {
        std::string expectedTypesStr;
        for (const auto &type: expectedTypes) {
            expectedTypesStr += tokenToString.at(type) + " ";
        }
        expectedTypesStr.pop_back(); // Remove trailing space
        error("Unexpected token, expected one of [" + expectedTypesStr + "], got " + currentToken.lexeme);
    }
}

void Parser::error(const std::string &message) {
    // Simple error reporting
    throw std::runtime_error("Parse error: " + message);
}

std::unique_ptr<Query> Parser::parseSelect() {
    auto query = std::make_unique<SelectQuery>();

    // Parse columns or * for all columns
    parseColumns(query);

    // Parse FROM clause
    parseFrom(query);

    // Parse WHERE clause
    parseWhere(query);

    return query;
}

void Parser::parseColumns(std::unique_ptr<SelectQuery> &query) {
    while (currentToken.type != TokenType::FROM) {
        if (currentToken.type == TokenType::END_OF_QUERY) {
            error("Unexpected end of query");
        }

        if (currentToken.type == TokenType::COMMA) {
            nextToken(); // Consume comma
        }

        if (currentToken.type == TokenType::IDENTIFIER || currentToken.type == TokenType::STAR) {
            query->columns.push_back(currentToken.lexeme);
            nextToken(); // Consume column name or *
        }
    }
    if (query->columns.empty()) {
        error("No columns specified");
    }
}

void Parser::parseFrom(std::unique_ptr<SelectQuery> &query) {
    // From is already in Parser
    expect({TokenType::FROM});
    nextToken(); // Consume FROM
    expect({TokenType::IDENTIFIER});
    query->fromTable = currentToken.lexeme;
    nextToken(); // Consume table name
}


ConditionGroup Parser::parseOrExpression() {
    ConditionGroup group(TokenType::OR);
    group.addConditionGroup(parseAndExpression());
    while (currentToken.type == TokenType::OR) {
        nextToken(); // Consume OR
        group.addConditionGroup(parseAndExpression());
    }
    return group;
}

ConditionGroup Parser::parseAndExpression() {
    ConditionGroup group(TokenType::AND);
    group.addConditionGroup(parseExpression());
    while (currentToken.type == TokenType::AND) {
        nextToken(); // Consume AND
        group.addConditionGroup(parseExpression());
    }
    return group;
}

ConditionGroup Parser::parseExpression() {
    if (currentToken.type == TokenType::LEFT_PAREN) {
        nextToken(); // Consume (
        ConditionGroup group = parseOrExpression();
        expect({TokenType::RIGHT_PAREN});
        nextToken(); // Consume )
        return group;
    } else {
        return parseCondition();
    }
}

ConditionGroup Parser::parseCondition() {
    ConditionGroup group(TokenType::AND);
    expect({TokenType::IDENTIFIER});
    std::string column = currentToken.lexeme;
    nextToken(); // Consume column name
    expect({TokenType::EQUAL, TokenType::LESS_THAN, TokenType::GREATER_THAN, TokenType::NOT_EQUAL, TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL
            , TokenType::IS_NULL, TokenType::IS_NOT_NULL});
    std::string op = currentToken.lexeme;
    if (currentToken.type == TokenType::IS_NULL || currentToken.type == TokenType::IS_NOT_NULL) {
        nextToken(); // Consume operator
        group.addCondition(Condition(column, op));
    } else {
        nextToken(); // Consume operator
        expect({TokenType::IDENTIFIER, TokenType::NUMBER, TokenType::STRING});
        std::string value = currentToken.lexeme;
        nextToken(); // Consume data
        group.addCondition(Condition(column, op, value));
    }
    return group;
}

void Parser::parseWhere(std::unique_ptr<SelectQuery> &query) {
    if (currentToken.type != TokenType::WHERE) {
        return;
    }
    nextToken(); // Consume WHERE
    query->whereClause = parseOrExpression();
    expect({TokenType::END_OF_QUERY});
}

std::unique_ptr<Query> Parser::parseInsert() {
    expect({TokenType::INTO});
    nextToken(); // Consume INTO

    std::string tableName = parseTableName();

    std::vector<std::string> columns = parseColumns();

    std::vector<std::string> values = parseValues();

    expect({TokenType::END_OF_QUERY});

    auto query = std::make_unique<InsertQuery>();
    query->tableName = tableName;
    query->columns = columns;
    query->values = values;

    return query;
}

std::string Parser::parseTableName() {
    expect({TokenType::IDENTIFIER});
    std::string tableName = currentToken.lexeme;
    nextToken(); // Consume table name
    return tableName;
}

std::vector<std::string> Parser::parseColumns() {
    expect({TokenType::LEFT_PAREN});
    nextToken(); // Consume (

    std::vector<std::string> columns;
    while (currentToken.type != TokenType::RIGHT_PAREN) {
        if (currentToken.type == TokenType::END_OF_QUERY) {
            error("Unexpected end of query");
        }
        if (currentToken.type == TokenType::COMMA) {
            nextToken(); // Consume comma
        }
        expect({TokenType::IDENTIFIER});
        columns.push_back(currentToken.lexeme);
        nextToken(); // Consume column name
    }

    nextToken(); // Consume )
    return columns;
}

std::vector<std::string> Parser::parseValues() {
    expect({TokenType::VALUES});
    nextToken(); // Consume VALUES

    expect({TokenType::LEFT_PAREN});
    nextToken(); // Consume (

    std::vector<std::string> values;
    while (currentToken.type != TokenType::RIGHT_PAREN) {
        if (currentToken.type == TokenType::END_OF_QUERY) {
            error("Unexpected end of query");
        }
        if (currentToken.type == TokenType::COMMA) {
            nextToken(); // Consume comma
        }
        expect({TokenType::IDENTIFIER, TokenType::NUMBER, TokenType::STRING});
        values.push_back(currentToken.lexeme);
        nextToken(); // Consume data
    }

    nextToken(); // Consume )
    return values;
}

std::unique_ptr<Query> Parser::parseCreate() {

    expect({TokenType::TABLE});
    nextToken(); // Consume TABLE

    std::string tableName = parseTableName();

    auto const &[columns, relations] = parseColumnDefinitionsAndRelations();

    expect({TokenType::END_OF_QUERY});

    auto query = std::make_unique<CreateTableQuery>();
    query->tableName = tableName;
    query->columns = columns;
    query->relations = relations;

    return query;
}

std::pair<std::vector<Column>, std::vector<ParsedRelation>> Parser::parseColumnDefinitionsAndRelations() {
    expect({TokenType::LEFT_PAREN});
    nextToken(); // Consume (

    std::vector<Column> columns;
    std::vector<ParsedRelation> relations;
    while (currentToken.type != TokenType::RIGHT_PAREN) {
        if (currentToken.type == TokenType::END_OF_QUERY) {
            error("Unexpected end of query");
        }
        if (currentToken.type == TokenType::COMMA) {
            nextToken(); // Consume comma
        }
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string columnName = currentToken.lexeme;
            nextToken(); // Consume column name

            expect({TokenType::IDENTIFIER});
            DataType columnType = DataTypeUtils::fromString(currentToken.lexeme);
            nextToken(); // Consume column type

            // Parse constraints (PK, NOT_NULL, UNIQUE, FK, etc.)
            std::vector<ColumnConstraint> columnConstraints;
            while (isColumnConstraint(currentToken.type)) {
                ColumnConstraint constraint = parseColumnConstraint();
                columnConstraints.push_back(constraint);
                nextToken(); // Consume constraint
            }

            // we need to push column as columns is a vector of Column
            columns.emplace_back(columnName, columnType, columnConstraints);
        } else if (isTableConstraint(currentToken.type)) {
            TableConstraint constraint = parseTableConstraint();
            nextToken(); // Consume constraint
            if (constraint == TableConstraint::FOREIGN_KEY) {
                relations.push_back(createRelation(constraint));
            } else {
                error("Unexpected token, expected a foreign key constraint");
            }
        } else {
            error("Unexpected token, expected a column definition or a table constraint");
        }
    }

    nextToken(); // Consume )

    return {columns, relations};
}

bool Parser::isColumnConstraint(TokenType tokenType) {
    return tokenType == TokenType::NOT_NULL || tokenType == TokenType::UNIQUE || tokenType == TokenType::PRIMARY_KEY;
}

bool Parser::isTableConstraint(TokenType tokenType) {
    return tokenType == TokenType::FOREIGN_KEY;
}

TableConstraint Parser::parseTableConstraint() {
    switch (currentToken.type) {
        case TokenType::FOREIGN_KEY:
            return TableConstraint::FOREIGN_KEY;
        default:
            error("Unexpected token, expected a table constraint");
    }
}

ColumnConstraint Parser::parseColumnConstraint() {
    switch (currentToken.type) {
        case TokenType::NOT_NULL:
            return ColumnConstraint::NOT_NULL;
        case TokenType::UNIQUE:
            return ColumnConstraint::UNIQUE;
        case TokenType::PRIMARY_KEY:
            return ColumnConstraint::PRIMARY_KEY;
        default:
            error("Unexpected token, expected a column constraint");
    }
}

ParsedRelation Parser::createRelation(const TableConstraint &constraint) {
    if (constraint != TableConstraint::FOREIGN_KEY) {
        error("Cannot create a relation from a non-foreign key constraint");
    }

    // Parse the name of the column
    expect({TokenType::IDENTIFIER});
    std::string columnName = currentToken.lexeme;
    nextToken(); // Consume column name

    // Parse the REFERENCES keyword
    expect({TokenType::REFERENCES});
    nextToken(); // Consume REFERENCES

    // Parse the name of the referenced table
    expect({TokenType::IDENTIFIER});
    std::string referencedTableName = currentToken.lexeme;
    nextToken(); // Consume table name

    // Parse the name of the referenced column
    expect({TokenType::IDENTIFIER});
    std::string referencedColumnName = currentToken.lexeme;
    nextToken(); // Consume column name

    // Create the RelationQuery object
    return {columnName, referencedTableName, referencedColumnName};
}

std::unique_ptr<Query> Parser::parseAlter() {
    expect({TokenType::TABLE});
    nextToken(); // Consumes TABLE

    std::string tableName = parseTableName();

    std::vector<std::variant<AddColumnOperation, DropColumnOperation, AddForeignKeyOperation>> operations;

    while (currentToken.type != TokenType::END_OF_QUERY) {
        if (currentToken.type == TokenType::ADD) {
            nextToken(); // Consumes ADD
            if (currentToken.type == TokenType::FOREIGN_KEY) {
                // Parse FOREIGN_KEY constraint
                nextToken(); // Consumes FOREIGN_KEY
                ParsedRelation relation = createRelation(TableConstraint::FOREIGN_KEY);
                operations.emplace_back(AddForeignKeyOperation(std::move(relation)));
            } else {
                // Parse column definition
                nextToken(); // Consumes COLUMN
                Column column = parseColumnDefinition();
                operations.emplace_back(AddColumnOperation(std::move(column)));
            }
        } else if (currentToken.type == TokenType::DROP) {
            nextToken(); // Consumes DROP
            expect({TokenType::COLUMN});
            nextToken(); // Consumes COLUMN
            std::string columnName = parseColumnName();
            operations.emplace_back(DropColumnOperation(std::move(columnName)));
        } else {
            expect({TokenType::ADD, TokenType::DROP});
        }
    }

    return std::make_unique<AlterTableQuery>(std::move(tableName), std::move(operations));
}


Column Parser::parseColumnDefinition() {
    // Parse the column name
    expect({TokenType::IDENTIFIER});
    std::string columnName = currentToken.lexeme;
    nextToken(); // Consume column name

    // Parse the data type
    expect({TokenType::IDENTIFIER});
    DataType dataType = DataTypeUtils::fromString(currentToken.lexeme);
    nextToken(); // Consume data type

    // Parse the column constraints
    std::vector<ColumnConstraint> constraints;
    while (isColumnConstraint(currentToken.type)) {
        ColumnConstraint constraint = parseColumnConstraint();
        constraints.push_back(constraint);
        nextToken(); // Consume constraint
    }

    return {columnName, dataType, constraints};
}

std::string Parser::parseColumnName() {
    // Parse the column name
    expect({TokenType::IDENTIFIER});
    std::string columnName = currentToken.lexeme;
    nextToken(); // Consume column name

    return columnName;
}


std::unique_ptr<Query> Parser::parseDrop() {
    // Expect the TABLE token
    expect({TokenType::TABLE});
    nextToken(); // Consume the TABLE token

    // Parse the table name
    std::string tableName = parseTableName();
    nextToken(); // Consume table name

    // Create and return a DropTableQuery object
    return std::make_unique<DropTableQuery>(std::move(tableName));
}