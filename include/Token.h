#ifndef INCLUDE_INCLUDE_TOKEN_H_
#define INCLUDE_INCLUDE_TOKEN_H_
#include <cstddef>
#include <string>
#include <variant>
enum class TokenType {
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

    IDENTIFIER,
    STRING,
    NUMBER,

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
    const std::string lexeme;
public:
    Token(TokenType type, std::string lexeme, std::variant<double, std::string, bool, nullptr_t> literal, int line);
    const int line;
    const TokenType type;
    std::variant<double, std::string, bool, nullptr_t> literal;
    [[nodiscard]] std::string typeToString() const;
    [[nodiscard]] std::string getLexeme() const;
    [[nodiscard]] std::string toString() const;
};

#endif // INCLUDE_INCLUDE_TOKEN_H_
