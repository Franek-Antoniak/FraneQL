#include "Lexer.h"

#include <cctype>
#include <utility>
#include <stdexcept>


void Lexer::reportError(const std::string& message) const {
    throw std::runtime_error("Lexer Error at position " + std::to_string(position) + ": " + message);
}

Lexer::Lexer(std::string input) : input(std::move(input)), position(0) {}

Token Lexer::nextToken() {
    skipWhitespace();

    if (isEnd()) return Token(TokenType::END_OF_QUERY, "; or end of file");

    char currentChar = peek();

    if (isAlpha(currentChar)) {
        return identifierOrKeyword();
    } else if (isDigit(currentChar)) {
        return number();
    } else if (currentChar == '\'' || currentChar == '\"') {
        return stringLiteral();
    } else if (currentChar == '>' || currentChar == '<' || currentChar == '=') {
        return comparisonOperator();
    } else {
        return singleCharToken(get());
    }
}

bool Lexer::isAlpha(char c) {
    return std::isalpha(c) || c == '_'; // Include underscore for SQL identifiers
}

bool Lexer::isDigit(char c) {
    return std::isdigit(c);
}

bool Lexer::isEnd() const {
    return isFileEnd() || isQueryEnd();
}

bool Lexer::isFileEnd() const {
    return position >= input.size() || input[position] == '\0';
}

bool Lexer::isQueryEnd() const {
    return input[position] == ';';
}

char Lexer::get() {
    if (isEnd()) return '\0';
    return input[position++];
}

char Lexer::peek() const {
    if (isEnd()) return '\0';
    return input[position];
}

void Lexer::skipWhitespace() {
    while (!isEnd() && std::isspace(peek())) {
        get(); // Consume the character
    }
}

Token Lexer::singleCharToken(char c) {
    auto found = keywords.find(std::string(1, c));
    if (found != keywords.end()) {
        return Token(found->second, std::string(1, c));
    }
    return Token(TokenType::UNKNOWN, std::string(1, c));
}

Token Lexer::stringLiteral() {
    char endChar = get(); // should be '\'' or '\"'
    std::string value;
    while (!isEnd() && peek() != endChar) {
        value += get();
    }
    get(); // Consume the closing quote
    return Token(TokenType::STRING, value);
}

Token Lexer::number() {
    std::string value;
    while (!isEnd() && (isDigit(peek()) || peek() == '.')) {
        if (peek() == '.' && value.find('.') != std::string::npos) {
            // If we encounter a second '.', it's not a valid number
            reportError("Invalid number format, too many decimal points");
            break;
        }
        value += get();
    }

    return Token(TokenType::NUMBER, value);
}

Token Lexer::comparisonOperator() {
    std::string value;
    value += get(); // get the first character ('>', '<', or '=')

    // Check if the next character is '=' or '>', forming a two-character operator
    if (!isEnd() && (peek() == '=' || (value[0] == '<' && peek() == '>'))) {
        value += get();
    }

    // Check if it's a known operator
    auto found = keywords.find(value);
    if (found != keywords.end()) {
        return Token(found->second, value);
    }

    return Token(TokenType::UNKNOWN, value);
}

Token Lexer::identifierOrKeyword() {
    std::string value;
    while (!isEnd() && (isAlpha(peek()) || isDigit(peek()) || peek() == '_')) {
        value += get();
    }

    // Check if it's a keyword
    auto found = keywords.find(value);
    if (found != keywords.end()) {
        return Token(found->second, value);
    }

    return Token(TokenType::IDENTIFIER, value);
}
