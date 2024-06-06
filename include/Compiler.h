#ifndef INCLUDE_COMPILER_H
#define INCLUDE_COMPILER_H
#include "AssemblyInfo.h"
#include "Environment.h"
#include "Expression.h"
#include "Statement.h"
#include "Token.h"

void generateLocalVariables(AssemblyInfo& info, Environment* environment);

class Compiler {
public:
    Environment environment = {};
    AssemblyInfo generateAssembly(const Expr& expr);
    AssemblyInfo generateAssembly(const Statement& stmt);

private:
    template <class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };
    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
    bool isTruthy(Expr& object);
    void checkNumberOperands(const Token& opr, const AssemblyInfo::Type& left, const AssemblyInfo::Type& right)
    {
        if (left == AssemblyInfo::Type::DOUBLE && right == AssemblyInfo::Type::DOUBLE)
            return;
        throw std::runtime_error("Operands must be numbers.");
    }

    void checkNumberOperand(const Token& opr, const AssemblyInfo::Type& right) const
    {
        if (right == AssemblyInfo::Type::DOUBLE)
            return;
        throw std::runtime_error("Operand must be a number.");
    }
};
#endif
