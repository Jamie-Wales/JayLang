#pragma once
#include "Expression.h"
#include <vector>

class Statement;

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

struct Block {
    std::vector<std::shared_ptr<Statement>> statements;
};

struct While {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Statement> body;
};
struct IfStatement {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Statement> ifBlock;
    std::shared_ptr<Statement> elseBlock;
};
