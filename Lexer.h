#pragma once

#include "Token.h"
#include <string>

class Lexer {
public:
    explicit Lexer(std::string input);

    virtual Token nextToken();

private:
    std::string input;
    size_t position = 0;

    virtual void skipWhitespace();
    virtual Token identifierOrKeyword();
    virtual Token number();
    virtual Token stringLiteral();
    static Token singleCharToken(char c);
    [[nodiscard]] virtual char peek() const;
    virtual char get();
    [[nodiscard]] virtual bool isEnd() const;
    [[nodiscard]] static bool isAlpha(char c) ;
    [[nodiscard]] static bool isDigit(char c) ;
    virtual void reportError(const std::string &message) const;

    [[nodiscard]] virtual bool isFileEnd() const;

    [[nodiscard]] virtual bool isQueryEnd() const;

    virtual Token comparisonOperator() ;
};