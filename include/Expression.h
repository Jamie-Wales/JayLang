#ifndef INCLUDE_INCLUDE_EXPRESSION_H_
#define INCLUDE_INCLUDE_EXPRESSION_H_

#include "Token.h"
#include <iostream>
#include <memory>
#include <variant>

enum class ExprType {
    LITERAL,
    UNARY,
    BINARY,
    GROUPING,
    TERNARY,
    VARIABLE,
    ASSIGNMENT
};

class Expr;

struct Binary {
    std::shared_ptr<Expr> left;
    const Token opr;
    std::shared_ptr<Expr> right;
};

struct Grouping {
    std::shared_ptr<Expr> expression;
};

struct Assign {
    const Token name;
    std::shared_ptr<Expr> value;
};

struct Unary {
    const Token opr;
    std::shared_ptr<Expr> value;
};

struct Ternary {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
};

struct Literal {
    const std::variant<double, std::string, bool, nullptr_t> literal;
};

struct Variable {
    Token name;
    std::shared_ptr<Expr> value;
};

class Expr {
public:
    ExprType type;
    std::variant<Unary,
        Binary, Assign, Grouping, Literal, Ternary, Variable>
        content;
    Expr(ExprType type, std::variant<Unary, Binary, Assign, Grouping, Literal, Ternary, Variable> content)
        : type(type)
        , content(std::move(content)) {};

    Expr(Expr& expression)
        : type { expression.type }
        , content { std::move(expression.content) } {};

    virtual ~Expr() = default;
};

std::ostream& operator<<(std::ostream& os, const Expr& expr);

#endif // INCLUDE_INCLUDE_EXPRESSION_H_
