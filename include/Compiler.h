#pragma once
#include "AssemblyInfo.h"
#include "Environment.h"
#include "Expression.h"
#include "Statement.h"
#include "Token.h"

class Compiler {
public:
    Environment* environment = new Environment();

    AssemblyInfo generateAssembly(const Expr& expr);

    AssemblyInfo generateAssembly(const Statement& stmt);

    std::string localVariableTable;

    void generateLocalVariables(AssemblyInfo& info, Environment* environment) const;

private:
    template <class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    AssemblyInfo JavaStaticCall(const std::vector<std::shared_ptr<Expr>> arguments);
    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    static bool isTruthy(const Expr& object);

    static void checkNumberOperands(const Token& opr, const AssemblyInfo::Type& left, const AssemblyInfo::Type& right)
    {
        if (left == AssemblyInfo::Type::DECIMAL && right == AssemblyInfo::Type::DECIMAL)
            return;
        throw std::runtime_error("Operands must be numbers.");
    }

    static void checkNumberOperand(const Token& opr, const AssemblyInfo::Type& right)
    {
        if (right == AssemblyInfo::Type::DECIMAL)
            return;
        throw std::runtime_error("Operand must be a number.");
    }

    int labelCounter = 0;

    std::string generateLabel();
    void emitLabel(std::string& code, const std::string& label);
    void emitJump(std::string& code, const std::string& instruction, const std::string& label);
    void emitInstruction(std::string& code, const std::string& instruction);
    void emitMethodCall(std::string& code, const std::string& className, const std::string& methodName, const std::string& descriptor, const bool& isStatic);

    auto generateLogicalOperation(const Logical& l) -> AssemblyInfo;
    auto generateIfElseStatement(const IfStatement& ifStmt) -> AssemblyInfo;
    auto generateWhileStatement(const While& w) -> AssemblyInfo;
    AssemblyInfo generateBytecode(const Binary& b);
    AssemblyInfo generateBytecode(const Logical& l);
    AssemblyInfo generateBytecode(const Unary& b);
};
