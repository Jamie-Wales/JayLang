#pragma once
#include "ErrorHandler.h"
#include "ParseError.h"
#include "Statement.h"
#include "Token.h"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
        : tokens { std::move(tokens) } {};

    std::vector<std::shared_ptr<Statement> > parse();

    ErrorHandler err = {};
    size_t current = 0;

private:
    ParseError error(Token& token, const std::string& message);

    std::vector<Token> tokens;

    std::shared_ptr<Expr> logicalOR();

    std::shared_ptr<Expr> logicalAND();

    std::shared_ptr<Expr> expression();

    std::shared_ptr<Expr> equality();

    std::shared_ptr<Expr> comparison();

    std::shared_ptr<Expr> term();

    std::shared_ptr<Expr> factor();

    std::shared_ptr<Expr> unary();

    std::shared_ptr<Expr> primary();

    std::shared_ptr<Expr> comma();

    std::shared_ptr<Expr> ternary();

    std::shared_ptr<Expr> variable();

    std::shared_ptr<Statement> statement();

    std::shared_ptr<Statement> declaration();

    /* Returns null for else block if else block does not exist */
    std::shared_ptr<Statement> ifStatement();

    std::shared_ptr<Statement> printStatement();

    std::shared_ptr<Statement> jjdeclaration();

    std::shared_ptr<Statement> expressionStatement();

    std::shared_ptr<Statement> blockStatement();

    std::shared_ptr<Expr> assignment();

    bool match(const std::initializer_list<TokenType>& types);

    Token& peek();

    [[nodiscard]] Token& consume() const;

    Token& consume(TokenType type, const std::string& message);

    Token& previous();

    bool check(TokenType type);

    Token& advance();

    bool isAtEnd();

    void synchronize();
};
