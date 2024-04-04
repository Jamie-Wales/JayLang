#ifndef INCLUDE_STATEMENT_H_
#define INCLUDE_STATEMENT_H_
#include "Expression.h"
#include "Token.h"
#include <memory>
#include <variant>

struct ExprStatement {
    std::shared_ptr<Expr> expression;
};

struct PrintStatement {
    std::shared_ptr<Expr> expression;
};

struct JJStatement {
    Token name;
    std::shared_ptr<Expr> value;
};

class Statement {
public:
    std::variant<ExprStatement, PrintStatement, JJStatement> content;
    Statement(std::variant<ExprStatement, PrintStatement, JJStatement> content)
        : content(std::move(content))
    {
    }
};

#endif
