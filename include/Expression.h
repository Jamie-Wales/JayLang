#ifndef INCLUDE_INCLUDE_EXPRESSION_H_
#define INCLUDE_INCLUDE_EXPRESSION_H_

#include "Token.h"
#include <memory>

/*
 *
 * EXPRESSIONS
 *
 *
 */

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
    const ::LiteralType type;
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

std::ostream& operator<<(std::ostream& os, const Expr& expr)
{
    switch (expr.type) {
    case ExprType::LITERAL: {
        Literal l = std::get<Literal>(expr.content);
        if (l.type == LiteralType::NUMBER) {
            os << std::to_string(*std::static_pointer_cast<double>(l.value));
        } else {
            os << *std::static_pointer_cast<std::string>(l.value);
        }
    } break;
    case ExprType::UNARY: {
        const Unary un = std::get<Unary>(expr.content);
        os << "(" << un.opr.getLexeme() << *un.value << ")";
    } break;
    case ExprType::BINARY: {
        const Binary bi = std::get<Binary>(expr.content);
        os << "(" << bi.opr.getLexeme() << *bi.left << *bi.right << ")";
    } break;
    case ExprType::GROUPING: {
        const Grouping gr = std::get<Grouping>(expr.content);
        os << "(" << *gr.expression << ")";
    } break;
    }

    return os;
};

#endif // INCLUDE_INCLUDE_EXPRESSION_H_
