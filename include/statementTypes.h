

#ifndef INCLUDE_STATEMENTTYPES_H_
#define INCLUDE_STATEMENTTYPES_H_

#include "Expression.h"
#include <memory>

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

class Statement;
struct Block {
    std::vector<std::shared_ptr<Statement>> statements;
};

#endif // INCLUDE_INCLUDE_STATEMENTTYPES_H_
