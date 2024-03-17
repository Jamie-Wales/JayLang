
#include "Parser.h"

std::shared_ptr<Expr>
Parser::parse()
{
    return expression();
};

std::shared_ptr<Expr> Parser::expression()
{
    return term();
};

std::shared_ptr<Expr> Parser::term()
{
    auto expr = factor();

    while (match({ TokenType::PLUS, TokenType::MINUS })) {

        Token opr = previous();
        auto right = factor();
        expr = std::make_shared<Expr>(ExprType::BINARY, Binary { expr, opr, right });
    };

    return expr;
};

std::shared_ptr<Expr> Parser::factor()
{
    auto expr = unary();
    if (match({ TokenType::BANG, TokenType::MINUS })) {
        Token oper = previous();
        auto right = unary();
        return std::make_shared<Expr>(ExprType::BINARY, Binary { expr, oper, right });
    }

    return primary();
}

std::shared_ptr<Expr> Parser::primary()
{
    std::string str;
    if (match({ TokenType::FALSE }))
        str = "FALSE";
    return std::make_shared<Expr>(ExprType::LITERAL, Literal { LiteralType::BOOLEAN, std::static_pointer_cast<void>(std::make_shared<std::string>("FALSE")) });
    if (match({ TokenType::TRUE }))
        return std::make_shared<Expr>(ExprType::LITERAL, Literal { LiteralType::BOOLEAN, std::static_pointer_cast<void>(std::make_shared<std::string>("TRUE")) });
    if (match({ TokenType::NIL }))
        return std::make_shared<Expr>(ExprType::LITERAL, Literal { LiteralType::BOOLEAN, std::static_pointer_cast<void>(std::make_shared<std::string>("NIL")) });
    if (match({ TokenType::STRING }))
        return std::make_shared<Expr>(ExprType::LITERAL, Literal { LiteralType::STRING, previous().literal });
    if (match({ TokenType::LEFT_PAREN })) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Expr>(ExprType::GROUPING, Grouping { expr });
    };
};

bool Parser::match(const std::initializer_list<TokenType>& types)
{
    for (auto& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
};

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;
    return tokens[current].type == type;
}

Token Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

bool Parser::isAtEnd()
{
    return current >= tokens.size();
};
