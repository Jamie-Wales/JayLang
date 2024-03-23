#ifndef INCLUDE_INCLUDE_SCANNER_H_
#define INCLUDE_INCLUDE_SCANNER_H_
#include "ErrorHandler.h"
#include "Token.h"
#include <string>

class Scanner {
private:
    const std::string source;
    std::vector<Token> tokens {};
    size_t start = 0;
    size_t current = 0;
    size_t line = 0;

    std::unordered_map<std::string, TokenType> keywords {
        { "and", TokenType::AND },
        { "class", TokenType::CLASS },
        { "else", TokenType::ELSE },
        { "false", TokenType::FALSE },
        { "for", TokenType::FOR },
        { "func", TokenType::FUNC },
        { "if", TokenType::IF },
        { "nil", TokenType::NIL },
        { "or", TokenType::OR },
        { "lg", TokenType::PRINT },
        { "return", TokenType::RETURN },
        { "super", TokenType::SUPER },
        { "this", TokenType::THIS },
        { "true", TokenType::TRUE },
        { "jj", TokenType::JJ },
        { "while", TokenType::WHILE },
    };
    char peek();
    char peekNext();
    bool isAtEnd();
    bool match(char expected);
    static bool isDigit(const char c)
    {
        return c >= '0' && c <= '9';
    }

    static bool isAlpha(const char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    static bool isAlphaNumberic(const char c)
    {
        return isAlpha(c) || isDigit(c);
    }

    void number();
    void string();
    void comment();
    char advance();
    void addToken(TokenType token, std::variant<double, std::string, bool, nullptr_t> literal);
    void addToken(TokenType token);
    void scanToken();

public:
    explicit Scanner(std::string source)
        : source {
            std::move(source)
        } {};
    std::vector<Token> scanTokens();
    ErrorHandler err = {};
};

#endif // INCLUDE_INCLUDE_SCANNER_H_if;
