#include "Expression.h"

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

std::ostream& operator<<(std::ostream& os, const Expr& expr)
{
    std::visit(overloaded {
                   [&](const Binary& b) { os << "(" << b.opr.getLexeme() << " " << *b.left << " " << *b.right << ")"; },
                   [&](const Unary& u) { os << "(" << u.opr.getLexeme() << " " << *u.value << ")"; },
                   [&](const Grouping& g) { os << "(" << *g.expression << ")"; },
                   [&](const Literal& l) {
                       std::visit(overloaded {
                                      [&](const std::string& s) { os << '"' << s << '"'; },
                                      [&](double d) { os << d; },
                                      [&](bool b) { os << (b ? "true" : "false"); },
                                      [&](auto&) { os << "unknown"; } // Handle other types, or unknown ones.
                                  },
                           l.literal);
                   },
                   [&](const Ternary& t) { os << "(" << *t.condition << " ? " << *t.left << " : " << *t.right << ")"; },
                   [&](auto&) { os << "unknown expr type"; } // Catch-all for any other types.
               },
        expr.content);

    return os;
}
