#include "Expression.h"

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
        os << "(" << un.opr.getLexeme() << " " << *un.value << ")";
    } break;
case ExprType::BINARY: {
    const Binary bi = std::get<Binary>(expr.content);
    os << "(" << bi.opr.getLexeme() << " " << *bi.left << " " << *bi.right << ")";
} break;
    case ExprType::GROUPING: {
        const Grouping gr = std::get<Grouping>(expr.content);
        os << "(" << *gr.expression << ")";
    } break;
    }

    return os;
};
