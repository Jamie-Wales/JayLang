#ifndef INCLUDE_INCLUDE_PARSER_H_
#define INCLUDE_INCLUDE_PARSER_H_
#include "ErrorHandler.h"
#include "Expression.h"
#include "ParseError.h"
#include "Token.h"
#include <memory>

class Parser {
public:
    Parser(std::vector<Token> tokens)
        : tokens { std::move(tokens) } {};
    std::shared_ptr<Expr> parse();
    ErrorHandler err = {};
    size_t current = 0;

private:
    ParseError error(Token& token, const std::string& message);
    std::vector<Token> tokens;
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> primary();
    std::shared_ptr<Expr> comma();
    std::shared_ptr<Expr> ternary();

    bool match(const std::initializer_list<TokenType>& types);
    Token& peek();
    Token& consume() const;
    Token& consume(TokenType type, const std::string& message);
    Token& previous();
    bool check(TokenType type);
    Token& advance();
    bool isAtEnd();
    void synchronize();
};

#endif // INCLUDE_INCLUDE_PARSER_H_
