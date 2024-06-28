#pragma once
#include "statementTypes.h"
#include <variant>

class Statement {
public:
    std::variant<ExprStatement, PrintStatement, JJStatement, Block, IfStatement, While, Function> content;

    explicit Statement(std::variant<ExprStatement, PrintStatement, JJStatement, Block, IfStatement, While, Function> content)
        : content(std::move(content))
    {
    }
};
