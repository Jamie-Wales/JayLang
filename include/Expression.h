#ifndef INCLUDE_INCLUDE_EXPRESSION_H_
#define INCLUDE_INCLUDE_EXPRESSION_H_

#include "Token.h"
#include <exception>
#include <iostream>
#include <memory>
enum class ExprType {
    LITERAL,
    UNARY,
    BINARY,
    GROUPING,
};

enum class LiteralType {
    STRING,
    NUMBER,
    BOOLEAN
};

#include <variant>

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
    std::unique_ptr<Expr> value;
};

struct Unary {
    const Token opr;
    std::shared_ptr<Expr> value;
};

struct Function {
    const Token name;
    const std::vector<std::shared_ptr<Expr>> params;
};

struct Literal {
    const LiteralType type;
    const std::shared_ptr<void> value;
};

class Expr {
public:
    ExprType type;
    std::variant<Unary,
        Binary, Assign, Grouping, Literal>
        content;

    Expr(ExprType type, std::variant<Unary, Binary, Assign, Grouping, Literal> content)

        : type(type)
        , content(std::move(content)) {};

    Expr(Expr& expression)
        : type { expression.type }
        , content { std::move(expression.content) } {};

    virtual ~Expr() = default;
};

std::ostream& operator<<(std::ostream& os, const Expr& expr);

#endif // INCLUDE_INCLUDE_EXPRESSION_H_
