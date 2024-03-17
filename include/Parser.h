#ifndef INCLUDE_INCLUDE_PARSER_H_
#define INCLUDE_INCLUDE_PARSER_H_
#include <Expression.h>
#include <Token.h>
#include <memory>
class Parser {
public:
    Parser(std::vector<Token> tokens)
        : tokens { std::move(tokens) } {};
    std::shared_ptr<Expr> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> primary();

    bool match(const std::initializer_list<TokenType>& types);
    Token& consume() const;
    Token& consume(TokenType type, const std::string& message);
    Token& previous() const;
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
};

#endif // INCLUDE_INCLUDE_PARSER_H_
