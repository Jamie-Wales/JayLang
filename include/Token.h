#ifndef INCLUDE_INCLUDE_TOKEN_H_
#define INCLUDE_INCLUDE_TOKEN_H_
#include <cstddef>
#include <optional>
#include <string>
#include <variant>
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
    QUESTION_MARK,
    COLON,

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
class Token {
private:
    const std::string lexeme;

public:
    Token(TokenType type, std::string lexeme, std::variant<double, std::string, bool, nullptr_t> literal, int line);
    const int line;
    const TokenType type;
    std::variant<double, std::string, bool, nullptr_t> literal;
    std::string typeToString() const;
    std::string getLexeme() const;
    std::string toString() const;
};

#endif // INCLUDE_INCLUDE_TOKEN_H_
