#include "Compiler.h"
#include "AssemblyInfo.h"
#include "Environment.h"
#include "Expression.h"
#include "Statement.h"
#include "statementTypes.h"
#include <cstddef>
#include <stdexcept>
#include <string>

std::string Compiler::generateLabel()
{
    return "L" + std::to_string(labelCounter++);
}

void Compiler::emitLabel(std::string& code, const std::string& label)
{
    code += label + ":\n";
}

void Compiler::emitJump(std::string& code, const std::string& instruction, const std::string& label)
{
    code += instruction + " " + label + "\n";
}

void Compiler::emitInstruction(std::string& code, const std::string& instruction)
{
    code += instruction + "\n";
}

void Compiler::emitMethodCall(std::string& code, const std::string& className, const std::string& methodName, const std::string& descriptor, const bool& isStatic)
{
    code += (isStatic ? "invokestatic " : "invokevirtual ") + className + "/" + methodName + descriptor + "\n";
}

auto Compiler::generateBytecode(const Binary& b) -> AssemblyInfo
{
    AssemblyInfo info;
    auto leftInfo = generateAssembly(*b.left);
    auto rightInfo = generateAssembly(*b.right);
    info.code += leftInfo.code;
    info.code += rightInfo.code;
    switch (b.opr.type) {
    case TokenType::GREATER:
        emitMethodCall(info.code, "Types/JayObject", "greaterThan", "(LTypes/JayObject;)Z", false);
        info.type = AssemblyInfo::Type::BOOL;
        break;
    case TokenType::GREATER_EQUAL:
        emitMethodCall(info.code, "Types/JayObject", "greaterThanEqual", "(LTypes/JayObject;)Z", false);
        info.type = AssemblyInfo::Type::BOOL;
        break;
    case TokenType::LESS:
        emitMethodCall(info.code, "Types/JayObject", "lessThan", "(LTypes/JayObject;)Z", false);
        info.type = AssemblyInfo::Type::BOOL;
        break;
    case TokenType::LESS_EQUAL:
        emitMethodCall(info.code, "Types/JayObject", "lessThanEqual", "(LTypes/JayObject;)Z", false);
        info.type = AssemblyInfo::Type::BOOL;
        break;
    case TokenType::MINUS:
        emitMethodCall(info.code, "Types/JayObject", "subtract", "(LTypes/JayObject;)LTypes/JayObject;", false);
        info.type = AssemblyInfo::Type::DECIMAL;
        break;
    case TokenType::SLASH:
        checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
        emitMethodCall(info.code, "Types/JayObject", "divide", "(LTypes/JayObject;)LTypes/JayObject;", false);
        info.type = AssemblyInfo::Type::DECIMAL;
        break;
    case TokenType::STAR:
        emitMethodCall(info.code, "Types/JayObject", "multiply", "(LTypes/JayObject;)LTypes/JayObject;", false);
        info.type = AssemblyInfo::Type::DECIMAL;
        break;
    case TokenType::PLUS:
        emitMethodCall(info.code, "Types/JayObject", "add", "(LTypes/JayObject;)LTypes/JayObject;", false);
        if (leftInfo.type == AssemblyInfo::Type::DECIMAL && rightInfo.type == AssemblyInfo::Type::DECIMAL) {
            info.type = AssemblyInfo::Type::DECIMAL;
        } else {
            info.type = AssemblyInfo::Type::STRING;
        }
        break;
    default:
        throw std::runtime_error("Unexpected binary operator");
    }
    return info;
}

auto Compiler::generateBytecode(const Unary& u) -> AssemblyInfo
{
    auto info = generateAssembly(*u.value);
    switch (u.opr.type) {
    case TokenType::MINUS:
        checkNumberOperand(u.opr, info.type);
        emitMethodCall(info.code, "Types/JayObject", "negate", "()LTypes/JayObject;", false);
        break;
    case TokenType::BANG:
        emitMethodCall(info.code, "Types/JayObject", "not", "()Z", false);
        info.type = AssemblyInfo::Type::BOOL;
        break;
    default:
        throw std::runtime_error("Unexpected unary operator");
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

auto Compiler::generateWhileStatement(const While& w) -> AssemblyInfo
{
    AssemblyInfo info;
    std::string conditionLabel = generateLabel();
    std::string endLabel = generateLabel();

    emitLabel(info.code, conditionLabel);

    auto condInfo = generateAssembly(*w.condition);
    info.code += condInfo.code;

    // The condition already leaves a boolean on the stack
    emitJump(info.code, "ifeq", endLabel);

    auto bodyInfo = generateAssembly(*w.body);
    info.code += bodyInfo.code;

    emitJump(info.code, "goto", conditionLabel);
    emitLabel(info.code, endLabel);

    return info;
}

auto Compiler::generateIfElseStatement(const IfStatement& ifStmt) -> AssemblyInfo
{
    AssemblyInfo info;
    auto conditionInfo = generateAssembly(*ifStmt.condition);
    info.code += conditionInfo.code;

    std::string elseLabel = generateLabel();
    std::string endLabel = generateLabel();

    emitJump(info.code, "ifeq", elseLabel);

    // If block
    auto ifBlockInfo = generateAssembly(*ifStmt.ifBlock);
    info.code += ifBlockInfo.code;
    emitJump(info.code, "goto", endLabel);

    // Else block
    emitLabel(info.code, elseLabel);
    if (ifStmt.elseBlock != nullptr) {
        auto elseBlockInfo = generateAssembly(*ifStmt.elseBlock);
        info.code += elseBlockInfo.code;
    }

    emitLabel(info.code, endLabel);

    return info;
}
auto Compiler::generateAssembly(const Statement& stmt) -> AssemblyInfo
{
    return std::visit(overloaded {
                          [&](const PrintStatement& ps) {
                              AssemblyInfo info = {};
                              emitInstruction(info.code, "getstatic java/lang/System/out Ljava/io/PrintStream;");
                              auto exprInfo = generateAssembly(*(ps.expression));
                              info.code += exprInfo.code;
                              emitMethodCall(info.code, "Types/JayObject", "toString", "()Ljava/lang/String;", false);
                              emitMethodCall(info.code, "java/io/PrintStream", "println", "(Ljava/lang/String;)V", false);
                              return info;
                          },
                          [&](const ExprStatement& es) {
                              return generateAssembly(*es.expression);
                          },
                          [&](const JJStatement& js) {
                              auto info = generateAssembly(*js.value);
                              environment->define(js.name.getLexeme(), info);
                              const auto element = environment->get(js.name.getLexeme());
                              emitInstruction(info.code, "astore " + std::to_string(element->index));
                              info.updateDepth(+1);
                              return info;
                          },
                          [&](const While& w) {
                              return generateWhileStatement(w);
                          },
                          [&](const Block& b) {
                              AssemblyInfo info = {};
                              Environment* env = environment->createChild();
                              environment = env;

                              std::string startLabel = generateLabel();
                              std::string endLabel = generateLabel();

                              emitLabel(info.code, startLabel);

                              for (const std::shared_ptr<Statement>& ptr : b.statements) {
                                  auto [code, maxStackDepth, currentDepth, type] = generateAssembly(*ptr);
                                  info.code += code;
                              }

                              for (auto& [name, variable] : env->variables) {
                                  localVariableTable += std::to_string(variable.index) + " is " + variable.name + "  Ljava/lang/String;" + " from " + startLabel + " to " + endLabel + "\n";
                              }

                              emitLabel(info.code, endLabel);

                              this->environment = this->environment->parent;
                              delete this->environment->child;
                              return info;
                          },
                          [&](const IfStatement& i) {
                              return generateIfElseStatement(i);
                          },
                          [&](auto&) {
                              throw std::runtime_error("Unsupported statement type");
                          } },
        stmt.content);
}

auto Compiler::generateAssembly(const Expr& expr) -> AssemblyInfo
{
    return std::visit(overloaded {
                          [&](const Literal& l) -> AssemblyInfo {
                              AssemblyInfo info;
                              std::visit(overloaded {
                                             [&](const double& d) {
                                                 emitInstruction(info.code, "ldc2_w " + std::to_string(d));
                                                 emitMethodCall(info.code, "Types/JayObject", "generateObject", "(D)LTypes/JayObject;", true);
                                                 info.updateDepth(1);
                                                 info.type = AssemblyInfo::Type::DECIMAL;
                                             },
                                             [&](const std::string& s) {
                                                 emitInstruction(info.code, "ldc " + s);
                                                 emitMethodCall(info.code, "Types/JayObject", "generateObject", "(Ljava/lang/String;)LTypes/JayObject;", true);
                                                 info.updateDepth(1);
                                                 info.type = AssemblyInfo::Type::STRING;
                                             },
                                             [&](const bool& b) {
                                                 emitInstruction(info.code, b ? "iconst_1" : "iconst_0");
                                                 emitMethodCall(info.code, "Types/JayObject", "generateObject", "(Z)LTypes/JayObject;", true);
                                                 info.updateDepth(2);
                                                 info.type = AssemblyInfo::Type::BOOL;
                                             },
                                             [&](const nullptr_t) {
                                                 emitInstruction(info.code, "aconst_null");
                                                 info.updateDepth(1);
                                                 info.type = AssemblyInfo::Type::NULL_T;
                                             },
                                             [&](auto&) { throw std::runtime_error("Undefined literal type"); } },
                                  l.literal);
                              return info;
                          },
                          [&](const Grouping& g) {
                              return generateAssembly(*g.expression);
                          },
                          [&](const Logical& l) -> AssemblyInfo {
                              return generateBytecode(Binary { l.left, l.token, l.right });
                          },
                          [&](const Unary& u) {
                              return generateBytecode(u);
                          },
                          [&](const Binary& b) {
                              return generateBytecode(b);
                          },
                          [&](const Variable& v) -> AssemblyInfo {
                              AssemblyInfo info;
                              const auto element = environment->get(v.name.getLexeme());
                              emitInstruction(info.code, "aload " + std::to_string(element->index));
                              info.type = element->info.type;
                              return info;
                          },
                          [&](const Assign& a) -> AssemblyInfo {
                              AssemblyInfo info = generateAssembly(*a.value);
                              const int index = environment->assign(a.name.getLexeme(), info);
                              emitInstruction(info.code, "astore " + std::to_string(index));
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
        const auto& l = std::get<Literal>(object.content);
        if (std::holds_alternative<nullptr_t>(l.literal))
            return false;
        if (std::holds_alternative<bool>(l.literal))
            return std::get<bool>(l.literal);
    }
    return true;
}
