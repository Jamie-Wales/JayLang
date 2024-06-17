#include "Expression.h"

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

std::ostream &operator<<(std::ostream &os, const Expr &expr) {
    std::visit(overloaded{
                   [&](const Binary &b) { os << "(" << b.opr.getLexeme() << " " << *b.left << " " << *b.right << ")"; },
                   [&](const Unary &u) { os << "(" << u.opr.getLexeme() << " " << *u.value << ")"; },
                   [&](const Grouping &g) { os << "(" << *g.expression << ")"; },
                   [&](const Literal &l) {
                       std::visit(overloaded{
                                      [&](const std::string &s) { os << '"' << s << '"'; },
                                      [&](const double d) { os << d; },
                                      [&](const bool b) { os << (b ? "true" : "false"); },
                                      [&](auto &) { os << "unknown"; } // Handle other types, or unknown ones.
                                  },
                                  l.literal);
                   },
                   [&](const Ternary &t) { os << "(" << *t.condition << " ? " << *t.left << " : " << *t.right << ")"; },
                   [&](auto &) { os << "unknown expr type"; } // Catch-all for any other types.
               },
               expr.content);

    return os;
};

std::string getLiteralType(const Expr &expr) {
    if (expr.type != ExprType::LITERAL) {
        return "Not a Literal";
    }

    const auto &lit = std::get<Literal>(expr.content);
    return std::visit([](auto &&arg) -> std::string {
                          using T = std::decay_t<decltype(arg)>;
                          if constexpr (std::is_same_v<T, double>) {
                              return "double";
                          } else if constexpr (std::is_same_v<T, std::string>) {
                              return "string";
                          } else if constexpr (std::is_same_v<T, bool>) {
                              return "bool";
                          } else if constexpr (std::is_same_v<T, nullptr_t>) {
                              return "nullptr";
                          } else {
                              return "unknown";
                          }
                      },
                      lit.literal);
}
