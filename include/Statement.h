#ifndef INCLUDE_STATEMENT_H_
#define INCLUDE_STATEMENT_H_

#include "statementTypes.h"
#include <variant>
class Statement {
public:
    std::variant<ExprStatement, PrintStatement, JJStatement, Block> content;
    explicit Statement(std::variant<ExprStatement, PrintStatement, JJStatement, Block> content)
        : content(std::move(content))
    {
    }
};

#endif
