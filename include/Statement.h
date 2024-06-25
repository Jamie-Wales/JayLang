#pragma once
#include "statementTypes.h"
#include <variant>

class Statement {
public:
    std::variant<ExprStatement, PrintStatement, JJStatement, Block, IfStatement, While> content;

    explicit Statement(std::variant<ExprStatement, PrintStatement, JJStatement, Block, IfStatement, While> content)
        : content(std::move(content))
    {
    }
};
