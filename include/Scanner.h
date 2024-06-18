#pragma once
#include "ErrorHandler.h"
#include "Token.h"
#include <string>
#include <unordered_map>
#include <vector>

class Scanner {
    const std::string source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    size_t line = 0;

    std::unordered_map<std::string, TokenType> keywords{
        {"and", TokenType::AND},
        {"class", TokenType::CLASS},
        {"else", TokenType::ELSE},
        {"false", TokenType::FALSE},
        {"for", TokenType::FOR},
        {"func", TokenType::FUNC},
        {"if", TokenType::IF},
        {"nil", TokenType::NIL},
        {"or", TokenType::OR},
        {"log", TokenType::LOG},
        {"return", TokenType::RETURN},
        {"super", TokenType::SUPER},
        {"this", TokenType::THIS},
        {"true", TokenType::TRUE},
        {"jj", TokenType::JJ},
        {"while", TokenType::WHILE},
    };

    char peek();

    [[nodiscard]] char peekNext() const;

    [[nodiscard]] bool isAtEnd() const;

    bool match(const char &expected);

    static bool isDigit(const char c) {
        return c >= '0' && c <= '9';
    }

    static bool isAlpha(const char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    static bool isAlphaNumberic(const char c) {
        return isAlpha(c) || isDigit(c);
    }

    void number();

    void string();

    void comment();

    char advance();

    void addToken(TokenType token, const std::variant<double, std::string, bool, nullptr_t> &literal);

    void addToken(TokenType token);

    void scanToken();

public:
    explicit Scanner(std::string source)
        : source{
            std::move(source)
        } {
    };

    std::vector<Token> scanTokens();

    ErrorHandler err = {};
};
