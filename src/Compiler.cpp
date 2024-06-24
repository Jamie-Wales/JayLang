#include "Compiler.h"
#include "AssemblyInfo.h"
#include "Environment.h"
#include "Expression.h"
#include "Statement.h"
#include "statementTypes.h"
#include <cstddef>
#include <string>

auto Compiler::generateBytecode(const Binary& b) -> AssemblyInfo
{
    AssemblyInfo info;
    auto leftInfo = generateAssembly(*b.left);
    auto rightInfo = generateAssembly(*b.right);
    info.code += leftInfo.code;
    info.code += rightInfo.code;
    switch (b.opr.type) {
    case TokenType::GREATER:
        info.code += "invokevirtual Types/JayObject/greaterThan(LTypes/JayObject;)Z\n";
        break;
    case TokenType::GREATER_EQUAL:
        info.code += "invokevirtual Types/JayObject/greaterThanEqual(LTypes/JayObject;)Z\n";
        break;
    case TokenType::LESS:
        info.code += "invokevirtual Types/JayObject/lessThan(LTypes/JayObject;)Z\n";
        break;
    case TokenType::LESS_EQUAL:
        info.code += "invokevirtual Types/JayObject/lessThanEqual(LTypes/JayObject;)Z\n";
        break;
    case TokenType::MINUS:
        info.code += "invokevirtual Types/JayObject/subtract(LTypes/JayObject;)LTypes/JayObject;\n";
        info.type = AssemblyInfo::Type::DECIMAL;
        break;
    case TokenType::SLASH:
        checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
        info.code += "invokevirtual java/math/BigDecimal/divide(Ljava/math/BigDecimal;)Ljava/math/BigDecimal;\n";
        info.type = AssemblyInfo::Type::DECIMAL;
        break;
    case TokenType::STAR:
        info.code += "invokevirtual Types/JayObject/multiply(LTypes/JayObject;)LTypes/JayObject;\n";
        info.type = AssemblyInfo::Type::DECIMAL;
        break;
    case TokenType::PLUS:
        if (leftInfo.type == AssemblyInfo::Type::DECIMAL && rightInfo.type == AssemblyInfo::Type::DECIMAL) {
            info.code += "invokevirtual Types/JayObject/add(LTypes/JayObject;)LTypes/JayObject;\n";
            info.type = AssemblyInfo::Type::DECIMAL;
        } else if (leftInfo.type == AssemblyInfo::Type::STRING && rightInfo.type == AssemblyInfo::Type::STRING) {
            info.code += "invokevirtual Types/JayObject/add(LTypes/JayObject;)LTypes/JayObject;\n";
            info.type = AssemblyInfo::Type::STRING;
        } else if (leftInfo.type == AssemblyInfo::Type::STRING && rightInfo.type == AssemblyInfo::Type::DECIMAL) {
            info.code += "invokevirtual Types/JayObject/add(LTypes/JayObject;)LTypes/JayObject;\n";
            info.type = AssemblyInfo::Type::STRING;
        } else if (leftInfo.type == AssemblyInfo::Type::DECIMAL && rightInfo.type == AssemblyInfo::Type::STRING) {
            info.code += "invokevirtual Types/JayObject/add(LTypes/JayObject;)LTypes/JayObject;\n";
            info.type = AssemblyInfo::Type::STRING;
        }
        break;
    }
    return info;
}

auto Compiler::generateBytecode(const Unary& u) -> AssemblyInfo
{
    auto info = generateAssembly(*u.value);
    switch (u.opr.type) {
    case TokenType::MINUS:
        checkNumberOperand(u.opr, info.type);
        info.code += "invokevirtual java/math/BigDecimal/negate()Ljava/math/BigDecimal;\n";
        break;
    case TokenType::BANG:
        isTruthy(*u.value) ? info.code += "iconst_0\n" : info.code += "iconst_1\n";
        break;
    }
    return info;
}

auto Compiler::generateLocalVariables(AssemblyInfo& info, [[maybe_unused]] Environment* environment) const -> void
{
    info.code += "return\n";
    info.code += ".localvariabletable\n";
    info.code += this->localVariableTable;
    info.code += ".end localvariabletable\n";
}

auto Compiler::generateAssembly(const Statement& stmt) -> AssemblyInfo
{
    return std::visit(overloaded {
                          [&](const PrintStatement& ps) {
                              AssemblyInfo info = {};
                              info.code += "getstatic java/lang/System/out Ljava/io/PrintStream;\n";
                              auto exprInfo = generateAssembly(*(ps.expression));
                              info.code += exprInfo.code;
                              info.code += "invokevirtual Types/JayObject/toString()Ljava/lang/String;\n";
                              info.code += "invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n";
                              return info;
                          },
                          [&](const ExprStatement& es) {
                              auto info = generateAssembly(*es.expression);
                              return info;
                          },
                          [&](const JJStatement& js) {
                              auto info = generateAssembly(*js.value);
                              environment->define(js.name.getLexeme(), info);
                              const auto element = environment->get(js.name.getLexeme());
                              info.code += "astore " + std::to_string(element->index) + "\n";
                              info.updateDepth(+1);
                              return info;
                          },
                          [&](const Block& b) {
                              AssemblyInfo info = {};

                              Environment* env = environment->createChild();
                              environment = env;
                              info.code += "L" + std::to_string(env->envindex) + ":\n";

                              for (const std::shared_ptr<Statement>& ptr : b.statements) {
                                  auto [code, maxStackDepth, currentDepth, type] = generateAssembly(*ptr);
                                  info.code += code;
                              }

                              for (auto& [name, variable] : env->variables) {
                                  localVariableTable += std::to_string(variable.index) + " is " + variable.name + "  Ljava/lang/String;" + " from L" + std::to_string(env->envindex) + " to L" + std::to_string(env->envindex + 1) + "\n";
                              }
                              info.code += "L" + std::to_string(++env->envindex) + ":\n";
                              this->environment = this->environment->parent;
                              delete this->environment->child;
                              return info;
                          },
                          [&](const IfStatement& i) -> AssemblyInfo {
                              auto info = generateAssembly(*i.condition);
                              if (i.elseBlock == nullptr) {
                                  info.code += "ifne L" + std::to_string(environment->envindex + 1) + "\n";
                                  info.code += "goto L" + std::to_string(environment->envindex + 2) + "\n";
                                  auto ifBlock = generateAssembly(*i.ifBlock);
                                  info.code += ifBlock.code;
                              } else {
                                  info.code += "ifne L" + std::to_string(environment->envindex + 3) + "\n";
                                  auto elseBlock = generateAssembly(*i.elseBlock);
                                  info.code += elseBlock.code;
                                  info.code += "goto L" + std::to_string(environment->envindex + 2) + "\n";
                                  auto ifBlock = generateAssembly(*i.ifBlock);
                                  info.code += ifBlock.code;
                              }
                              return info;
                          },
                          [&](auto&) {
                              throw std::runtime_error("Unsupported statement type");
                          } },
        stmt.content);
};

auto Compiler::generateAssembly(const Expr& expr) -> AssemblyInfo
{
    return std::visit(overloaded {
                          [&](const Literal& l) -> AssemblyInfo {
                              AssemblyInfo info;
                              std::visit(overloaded {
                                             [&](const double& d) {
                                                 info.code += "ldc2_w " + std::to_string(d) + "\n";
                                                 info.code += "invokestatic Types/JayObject/generateObject(D)LTypes/JayObject;\n";
                                                 info.updateDepth(1);
                                                 info.type = AssemblyInfo::Type::DECIMAL;
                                             },
                                             [&](const std::string& s) {
                                                 info.code += "ldc " + s + "\n";
                                                 info.code += "invokestatic Types/JayObject/generateObject(Ljava/lang/String;)LTypes/JayObject;\n";
                                                 info.updateDepth(1);
                                                 info.type = AssemblyInfo::Type::STRING;
                                             },
                                             [&](const bool& b) {
                                                 info.code = b ? "iconst_1\n" : "iconst_0\n";
                                                 info.updateDepth(2);
                                                 info.type = AssemblyInfo::Type::BOOL;
                                             },
                                             [&](const nullptr_t null) {
                                                 info.code = "aconst_null\n";
                                                 info.updateDepth(1);
                                                 info.type = AssemblyInfo::Type::NULL_T;
                                             },
                                             [&](auto&) { std::cerr << "Undefined expression" << std::endl; } },
                                  l.literal);
                              return info;
                          },
                          [&](const Grouping& g) -> AssemblyInfo {
                              auto info = generateAssembly(*g.expression);
                              return info;
                          },
                          [&](const Unary& u) -> AssemblyInfo {
                              return generateBytecode(u);
                          },
                          [&](const Binary& b) -> AssemblyInfo {
                              return generateBytecode(b);
                          },
                          [&](const Variable& v) -> AssemblyInfo {
                              AssemblyInfo info;
                              const auto element = environment->get(v.name.getLexeme());
                              info.code += "aload " + std::to_string(element->index) + "\n";
                              info.type = element->info.type;
                              return info;
                          },
                          [&](const Assign& a) -> AssemblyInfo {
                              AssemblyInfo info = generateAssembly(*a.value);
                              const int index = environment->assign(a.name.getLexeme(), info);
                              info.code += "astore " + std::to_string(index) + "\n";
                              return info;
                          },
                          [&](auto&) -> AssemblyInfo {
                              throw std::runtime_error("Unsupported expression type");
                          } },
        expr.content);
}

auto Compiler::isTruthy(const Expr& object) -> bool
{
    if (std::holds_alternative<Literal>(object.content)) {
        const auto l = std::get<Literal>(object.content);
        if (std::holds_alternative<nullptr_t>(l.literal))
            return false;
        if (std::holds_alternative<bool>(l.literal))
            return std::get<bool>(l.literal);
    }

    return true;
}
