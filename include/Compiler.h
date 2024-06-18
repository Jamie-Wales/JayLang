#ifndef INCLUDE_COMPILER_H
#define INCLUDE_COMPILER_H
#include "AssemblyInfo.h"
#include "Environment.h"
#include "Expression.h"
#include "Statement.h"
#include "Token.h"

class Compiler {
public:
    Environment *environment = new Environment();

    AssemblyInfo generateAssembly(const Expr &expr);

    AssemblyInfo generateAssembly(const Statement &stmt);

    std::string localVariableTable;

    void generateLocalVariables(AssemblyInfo &info, Environment *environment) const;

private:
    template<class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    bool isTruthy(Expr &object);

    static void checkNumberOperands(const Token &opr, const AssemblyInfo::Type &left, const AssemblyInfo::Type &right) {
        if (left == AssemblyInfo::Type::DOUBLE && right == AssemblyInfo::Type::DOUBLE)
            return;
        throw std::runtime_error("Operands must be numbers.");
    }

    static void checkNumberOperand(const Token &opr, const AssemblyInfo::Type &right) {
        if (right == AssemblyInfo::Type::DOUBLE)
            return;
        throw std::runtime_error("Operand must be a number.");
    }

    AssemblyInfo generateBytecode(const Binary &b);

    AssemblyInfo generateBytecode(const Unary &b);
};
#endif
