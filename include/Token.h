#ifndef INCLUDE_INCLUDE_TOKEN_H_
#define INCLUDE_INCLUDE_TOKEN_H_
#include <iostream>
#include <sstream>
#include <string>
enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUNC,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    JJ,
    WHILE,

    NONE,
    ENDOFFILE
};

/*
 *
 *  TOKEN
 *
 *
 */
class Token {
private:
    const std::string lexeme;

public:
    const int line;
    const TokenType type;
    const std::shared_ptr<void> literal;
    Token(TokenType type, std::string lexeme, std::shared_ptr<void> literal, int line)
        : lexeme(std::move(lexeme))
        , line(line)
        , type(type)
        , literal(std::move(literal)) {};

    std::string typeToString() const;
    std::string getLexeme() const;
    std::string toString() const;
};

#endif // INCLUDE_INCLUDE_TOKEN_H_
