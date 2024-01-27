#pragma once

#include <string>
#include <map>
#include <unordered_map>

// Credits to: https://stackoverflow.com/a/49595815/13292898

#define TOKEN_TYPES \
X(SELECT, "SELECT") \
X(WHERE, "WHERE")   \
X(INSERT, "INSERT") \
X(CREATE, "CREATE") \
X(TABLE, "TABLE")   \
X(STAR, "STAR")     \
X(VALUES, "VALUES") \
X(FROM, "FROM")     \
X(IDENTIFIER, "IDENTIFIER") \
X(STRING, "STRING") \
X(NUMBER, "NUMBER") \
X(PLUS, "PLUS")     \
X(MINUS, "MINUS")   \
X(EQUAL, "EQUAL")   \
X(LESS_THAN, "LESS_THAN")   \
X(GREATER_THAN, "GREATER_THAN") \
X(LESS_EQUAL, "LESS_EQUAL") \
X(GREATER_EQUAL, "GREATER_EQUAL") \
X(NOT_EQUAL, "NOT_EQUAL")   \
X(COMMA, "COMMA")   \
X(SEMICOLON, "SEMICOLON")   \
X(LEFT_PAREN, "LEFT_PAREN") \
X(RIGHT_PAREN, "RIGHT_PAREN")   \
X(AND, "AND")       \
X(OR, "OR")         \
X(UNKNOWN, "UNKNOWN")       \
X(END_OF_QUERY, "END_OF_QUERY") \
X(INTO, "INTO")     \
X(PRIMARY_KEY, "PRIMARY_KEY") \
X(FOREIGN_KEY, "FOREIGN_KEY") \
X(REFERENCES, "REFERENCES") \
X(NOT_NULL, "NOT_NULL") \
X(UNIQUE, "UNIQUE") \
X(IS_NULL, "IS_NULL")       \
X(IS_NOT_NULL, "IS_NOT_NULL")   \
X(ALTER, "ALTER")   \
X(ADD, "ADD")       \
X(DROP, "DROP")     \
X(COLUMN, "COLUMN")



#define X(enum_type, enum_name) enum_type,
enum class TokenType {
    TOKEN_TYPES
};

#undef X

#define X(enum_type, enum_name) {TokenType::enum_type, enum_name},
static const std::unordered_map<TokenType, std::string> tokenToString = {
        TOKEN_TYPES
};
#undef X

// We treat semicolon as a special token to indicate the end of a query
// It is the same as the end of file token

static inline const std::unordered_map<std::string, TokenType> keywords = {
        {"SELECT", TokenType::SELECT},
        {"WHERE", TokenType::WHERE},
        {"INSERT", TokenType::INSERT},
        {"CREATE", TokenType::CREATE},
        {"TABLE", TokenType::TABLE},
        {"VALUES", TokenType::VALUES},
        {"AND", TokenType::AND},
        {"OR", TokenType::OR},
        {"*", TokenType::STAR},
        {"=", TokenType::EQUAL},
        {"<", TokenType::LESS_THAN},
        {">", TokenType::GREATER_THAN},
        {"<=", TokenType::LESS_EQUAL},
        {">=", TokenType::GREATER_EQUAL},
        {"<>", TokenType::NOT_EQUAL},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {",", TokenType::COMMA},
        {";", TokenType::SEMICOLON},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {"FROM", TokenType::FROM},
        {"INTO", TokenType::INTO} ,
        {"PRIMARY_KEY", TokenType::PRIMARY_KEY},
        {"FOREIGN_KEY", TokenType::FOREIGN_KEY},
        {"REFERENCES", TokenType::REFERENCES},
        {"NOT_NULL", TokenType::NOT_NULL},
        {"UNIQUE", TokenType::UNIQUE},
        {"IS_NULL", TokenType::IS_NULL},
        {"IS_NOT_NULL", TokenType::IS_NOT_NULL},
        {"ALTER", TokenType::ALTER},
        {"ADD", TokenType::ADD},
        {"DROP", TokenType::DROP},
        {"COLUMN", TokenType::COLUMN}
};


struct Token {
    TokenType type;
    std::string lexeme;
};


