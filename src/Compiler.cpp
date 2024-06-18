#include "Compiler.h"
#include "AssemblyInfo.h"
#include "Environment.h"
#include "Expression.h"
#include "Statement.h"
#include "statementTypes.h"
#include <cstddef>
#include <string>


auto Compiler::generateBytecode(const Binary &b) -> AssemblyInfo {
    AssemblyInfo info;
    auto leftInfo = generateAssembly(*b.left);
    auto rightInfo = generateAssembly(*b.right);
    info.code += leftInfo.code;
    info.code += rightInfo.code;
    switch (b.opr.type) {
        case TokenType::GREATER:
            checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
            info.code += "invokevirtual Types/JayObject/greaterThan(LTypes/JayObject;)Z\n";
            break;
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
            info.code += "invokevirtual Types/JayObject/greaterThanEqual(LTypes/JayObject;)Z\n";
            break;
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
            checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
            info.code += leftInfo.code;
            info.code += "invokevirtual java/math/BigDecimal/compareTo(Ljava/math/BigDecimal;)I\n";
            break;
        case TokenType::MINUS:
            checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
            info.code += leftInfo.code;
            info.code += "invokevirtual java/math/BigDecimal/subtract(Ljava/math/BigDecimal;)Ljava/math/BigDecimal;\n";
            info.type = AssemblyInfo::Type::DECIMAL;
            break;
        case TokenType::SLASH:
            checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
            info.code += "invokevirtual java/math/BigDecimal/divide(Ljava/math/BigDecimal;)Ljava/math/BigDecimal;\n";
            info.type = AssemblyInfo::Type::DECIMAL;
            break;
        case TokenType::STAR:
            checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
            info.code += "invokevirtual java/math/BigDecimal/multiply(Ljava/math/BigDecimal;)Ljava/math/BigDecimal;\n";
            info.type = AssemblyInfo::Type::DECIMAL;
            break;
        case TokenType::PLUS:
            if (leftInfo.type == AssemblyInfo::Type::DECIMAL && rightInfo.type == AssemblyInfo::Type::DECIMAL) {
                info.code += "invokevirtual java/math/BigDecimal/add(Ljava/math/BigDecimal;)Ljava/math/BigDecimal;\n";
                info.type = AssemblyInfo::Type::DECIMAL;
            } else if (leftInfo.type == AssemblyInfo::Type::STRING && rightInfo.type == AssemblyInfo::Type::STRING) {
                info.code += leftInfo.code + rightInfo.code +
                        "invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
                info.type = AssemblyInfo::Type::STRING;
            } else if (leftInfo.type == AssemblyInfo::Type::STRING && rightInfo.type ==
                       AssemblyInfo::Type::DECIMAL) {
                info.code += "invokevirtual java/math/BigDecimal/toString()Ljava/lang/String;\n";
                info.code += "invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
                info.type = AssemblyInfo::Type::STRING;
            } else if (leftInfo.type == AssemblyInfo::Type::DECIMAL && rightInfo.type ==
                       AssemblyInfo::Type::STRING) {
                info.code += "swap\n";
                info.code += "invokevirtual java/math/BigDecimal/toString()Ljava/lang/String;\n";
                info.code += "swap\n";
                info.code += "invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
                info.type = AssemblyInfo::Type::STRING;
            }
            break;
    }
    return info;
}

AssemblyInfo Compiler::generateBytecode(const Unary &u) {
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

void Compiler::generateLocalVariables(AssemblyInfo &info, [[maybe_unused]] Environment *environment) const {
    info.code += "return\n";
    info.code += ".localvariabletable\n";
    info.code += this->localVariableTable;
    info.code += ".end localvariabletable\n";
}

AssemblyInfo Compiler::generateAssembly(const Statement &stmt) {
    return std::visit(overloaded{
                          [&](const PrintStatement &ps) {
                              AssemblyInfo info = {};
                              info.code += "getstatic java/lang/System/out Ljava/io/PrintStream;\n";
                              auto exprInfo = generateAssembly(*(ps.expression));
                              info.code += exprInfo.code;
                              info.type = exprInfo.type;
                              if (info.type == AssemblyInfo::Type::DECIMAL) {
                                  info.code += "invokevirtual java/math/BigDecimal/toString()Ljava/lang/String;\n";
                              } else if (info.type == AssemblyInfo::Type::BOOL) {
                                  info.code += "invokestatic java/lang/Boolean/toString()Ljava/lang/String;\n";
                              }
                              info.code += "invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n";
                              return info;
                          },
                          [&](const ExprStatement &es) {
                              auto info = generateAssembly(*es.expression);
                              return info;
                          },
                          [&](const JJStatement &js) {
                              auto info = generateAssembly(*js.value);
                              environment->define(js.name.getLexeme(), info);
                              const auto element = environment->get(js.name.getLexeme());
                              info.code += "astore " + std::to_string(element->index) + "\n";
                              info.updateDepth(+1);
                              return info;
                          },
                          [&](const Block &b) {
                              AssemblyInfo info = {};

                              Environment *env = environment->createChild();
                              environment = env;
                              info.code += "L" + std::to_string(env->envindex) + ":\n";

                              for (const std::shared_ptr<Statement> &ptr: b.statements) {
                                  auto [code, maxStackDepth, currentDepth, type] = generateAssembly(*ptr);
                                  info.code += code;
                              }

                              for (auto &[name, variable]: env->variables) {
                                  localVariableTable += std::to_string(variable.index) + " is " + variable.name +
                                          "  Ljava/lang/String;" + " from L" + std::to_string(env->envindex) + " to L" +
                                          std::to_string(env->envindex + 1) + "\n";
                              }
                              info.code += "L" + std::to_string(env->envindex + 1) + ":\n";
                              this->environment = this->environment->parent;
                              delete this->environment->child;
                              return info;
                          },
                          [&](const IfStatement &i) -> AssemblyInfo {
                              const auto info = generateAssembly(*i.condition);
                              const auto ifBlock = generateAssembly(*i.ifBlock);
                              const auto elseBlock = i.elseBlock != nullptr
                                                         ? generateAssembly(*i.elseBlock)
                                                         : AssemblyInfo{};
                              return info;
                          },
                          [&](auto &) {
                              throw std::runtime_error("Unsupported statement type");
                          }
                      },
                      stmt.content);
};

AssemblyInfo Compiler::generateAssembly(const Expr &expr) {
    return std::visit(overloaded{
                          [&](const Literal &l) -> AssemblyInfo {
                              AssemblyInfo info;
                              std::visit(overloaded{
                                             [&](const double &d) {
                                                 info.code += "ldc2_w " + std::to_string(d) + "\n";
                                                 info.code +=
                                                         "invokestatic java/math/BigDecimal/valueOf(D)Ljava/math/BigDecimal;\n";
                                                 info.updateDepth(2);
                                                 info.type = AssemblyInfo::Type::DECIMAL;
                                             },
                                             [&](const std::string &s) {
                                                 info.code = "ldc " + s + "\n";
                                                 info.updateDepth(2);
                                                 info.type = AssemblyInfo::Type::STRING;
                                             },
                                             [&](const bool &b) {
                                                 info.code = b ? "iconst_1\n" : "iconst_0\n";
                                                 info.updateDepth(2);
                                                 info.type = AssemblyInfo::Type::BOOL;
                                             },
                                             [&](const nullptr_t null) {
                                                 info.code = "aconst_null\n";
                                                 info.updateDepth(1);
                                                 info.type = AssemblyInfo::Type::NULL_T;
                                             },
                                             [&](auto &) { std::cerr << "Undefined expression" << std::endl; }
                                         },
                                         l.literal);
                              return info;
                          },
                          [&](const Grouping &g) -> AssemblyInfo {
                              auto info = generateAssembly(*g.expression);
                              return info;
                          },
                          [&](const Unary &u) -> AssemblyInfo {
                              return generateBytecode(u);
                          },
                          [&](const Binary &b) -> AssemblyInfo {
                              return generateBytecode(b);
                          },
                          [&](const Variable &v) -> AssemblyInfo {
                              AssemblyInfo info;
                              const auto element = environment->get(v.name.getLexeme());
                              info.code += "aload " + std::to_string(element->index) + "\n";
                              info.type = element->info.type;
                              return info;
                          },
                          [&](const Assign &a) -> AssemblyInfo {
                              AssemblyInfo info = generateAssembly(*a.value);
                              const int index = environment->assign(a.name.getLexeme(), info);
                              info.code += "astore " + std::to_string(index) + "\n";
                              return info;
                          },
                          [&](auto &) -> AssemblyInfo {
                              throw std::runtime_error("Unsupported expression type");
                          }
                      },
                      expr.content);
}

bool Compiler::isTruthy(const Expr &object) {
    if (std::holds_alternative<Literal>(object.content)) {
        const auto l = std::get<Literal>(object.content);
        if (std::holds_alternative<nullptr_t>(l.literal))
            return false;
        if (std::holds_alternative<bool>(l.literal))
            return std::get<bool>(l.literal);
    }
    return true;
}
