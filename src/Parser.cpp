#include "Parser.h"
#include "Expression.h"
#include "Token.h"
#include <memory>

std::shared_ptr<Expr> Parser::parse()
{
    try {
        return expression();
    } catch (ParseError& error) {
        err.handlerError(peek().line, error.what());
        synchronize();
        return nullptr;
    }
};

std::shared_ptr<Expr> Parser::expression()
{
    return comma();
};

std::shared_ptr<Expr> Parser::comma()
{
    auto expr = ternary();
    while (match({ TokenType::COMMA })) {
        auto opr = previous();
        auto right = ternary();
        expr = std::make_shared<Expr>(ExprType::BINARY, Binary { expr, opr, right });
    };

    return expr;
};

std::shared_ptr<Expr> Parser::ternary()
{
    auto condition = equality();
    if (match({ TokenType::QUESTION_MARK })) {
        auto oper = previous();
        auto trueBranch = equality();
        consume(TokenType::COLON, "Expect ':' after expression.");
        auto falseBranch = equality();
        return std::make_shared<Expr>(ExprType::TERNARY, Ternary { condition, trueBranch, falseBranch });
    }

    return condition;
}

std::shared_ptr<Expr> Parser::equality()
{
    auto expr = comparison();

    while (match({ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL })) {

        auto opr = previous();
        auto right = comparison();
        expr = std::make_shared<Expr>(ExprType::BINARY, Binary { expr, opr, right });
    };

    return expr;
};

std::shared_ptr<Expr> Parser::comparison()
{
    auto expr = term();

    while (match({ TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL })) {

        Token opr = previous();
        auto right = term();
        expr = std::make_shared<Expr>(ExprType::BINARY, Binary { expr, opr, right });
    };

    return expr;
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
    while (match({ TokenType::SLASH, TokenType::STAR })) {
        Token oper = previous();
        auto right = unary();
        expr = std::make_shared<Expr>(ExprType::BINARY, Binary { expr, oper, right });
    }

    return expr; // Return the built-up expression.
}

std::shared_ptr<Expr> Parser::unary()
{
    while (match({ TokenType::BANG, TokenType::MINUS })) {
        Token oper = previous();
        auto right = unary();
        return std::make_shared<Expr>(ExprType::UNARY, Unary { oper, right });
    }

    return primary();
}

std::shared_ptr<Expr> Parser::primary()
{
    if (match({ TokenType::FALSE }))
        return std::make_shared<Expr>(ExprType::LITERAL, Literal { true });
    if (match({ TokenType::TRUE }))
        return std::make_shared<Expr>(ExprType::LITERAL, Literal { false });
    if (match({ TokenType::NIL }))
        return std::make_shared<Expr>(ExprType::LITERAL, Literal { nullptr });
    if (match({ TokenType::NUMBER }))
        return std::make_shared<Expr>(ExprType::LITERAL, Literal { previous().literal });
    if (match({ TokenType::STRING }))
        return std::make_shared<Expr>(ExprType::LITERAL, Literal { previous().literal });
    if (match({ TokenType::LEFT_PAREN })) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Expr>(ExprType::GROUPING, Grouping { expr });
    };
    throw ParseError { peek(), "unexpected expression" };
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

Token& Parser::previous()
{
    return tokens.at(current - 1);
}
Token& Parser::consume(TokenType type, const std::string& message)
{
    if (check(type))
        return advance();
    throw error(peek(), message);
}

ParseError Parser::error(Token& token, const std::string& message)
{
    err.handlerError(token.line, message);
    return ParseError(token, message);
};

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;
    return tokens[current].type == type;
}
Token& Parser::peek()
{
    return tokens.at(current);
}
Token& Parser::advance()
{
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::ENDOFFILE;
};

void Parser::synchronize()
{
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUNC:
        case TokenType::JJ:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        }

        advance();
    }
}
