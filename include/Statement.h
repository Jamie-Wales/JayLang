#pragma once
#include "statementTypes.h"
#include <variant>

class Statement {
public:
    std::variant<ExprStatement, PrintStatement, JJStatement, Block, IfStatement> content;

    explicit Statement(std::variant<ExprStatement, PrintStatement, JJStatement, Block, IfStatement> content)
        : content(std::move(content)) {
    }
};