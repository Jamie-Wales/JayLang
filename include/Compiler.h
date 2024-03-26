#ifndef INCLUDE_COMPILER_H
#define INCLUDE_COMPILER_H

#include "Expression.h"
#include <cstddef>
#include <string>
#include <variant>
struct AssemblyInfo {
    std::string code;
    size_t maxStackDepth = 0;
    size_t currentDepth = 0;
    enum class Type { DOUBLE,
        STRING,
        BOOL };

    Type type;
    void updateDepth(size_t depthNeeded)
    {
        currentDepth += depthNeeded;
        if (currentDepth > maxStackDepth) {
            maxStackDepth = currentDepth;
        }
    }
    void consume(size_t depthUsed)
    {
        if (depthUsed <= currentDepth) {
            currentDepth -= depthUsed;
        } else {
            currentDepth = 0;
        }
    }
};

class Compiler {
public:
    AssemblyInfo generateAssembly(const Expr& expr)
    {
        return std::visit(overloaded {
                              [&](Grouping g) -> AssemblyInfo {
                                  auto info = generateAssembly(*g.expression);
                                  return info;
                              },
                              [&](const Unary& u) -> AssemblyInfo {
                                  auto info = generateAssembly(*u.value);
                                  info.code += "ineg\n";
                                  return info;
                              },
                              [&](const Binary& b) -> AssemblyInfo {
                                  auto leftInfo = generateAssembly(*b.left);
                                  auto rightInfo = generateAssembly(*b.right);

                                  AssemblyInfo info;
                                  info.code = leftInfo.code + rightInfo.code;
                                  if (leftInfo.type == AssemblyInfo::Type::DOUBLE || rightInfo.type == AssemblyInfo::Type::DOUBLE) {
                                      if (b.opr.type == TokenType::PLUS) {
                                          info.code += "dadd\n";
                                      } else if (b.opr.type == TokenType::MINUS) {
                                          info.code += "dsub\n";
                                      } else if (b.opr.type == TokenType::STAR) {
                                          info.code += "dmul\n";
                                      } else if (b.opr.type == TokenType::SLASH) {
                                          info.code += "ddiv\n";
                                      } else if (b.opr.type == TokenType::GREATER) {
                                          info.code += "if_icmpgt L1\niconst_0\nL1:\n";
                                          info.updateDepth(1);
                                      } else if (b.opr.type == TokenType::GREATER_EQUAL) {
                                          info.code += "if_icmpge L1\niconst_0\nL1:\n";
                                          info.updateDepth(1);
                                      } else if (b.opr.type == TokenType::LESS) {
                                          info.code += "if_icmplt L1\niconst_0\nL1:\n";
                                          info.updateDepth(1);
                                      } else if (b.opr.type == TokenType::LESS_EQUAL) {
                                          info.code += "if_icmple L1\niconst_0\nL1:\n";
                                          info.updateDepth(1);
                                      } else if (b.opr.type == TokenType::EQUAL_EQUAL) {
                                          info.code += "if_icmpeq L1\niconst_0\nL1:\n";
                                          info.updateDepth(1);
                                      } else if (b.opr.type == TokenType::BANG_EQUAL) {
                                          info.code += "if_icmpne L1\niconst_0\nL1:\n";
                                          info.updateDepth(1);
                                      }
                                  }
                                  info.updateDepth(2);
                                  return info;
                              },
                              [&](const Literal& l) -> AssemblyInfo {
                                  AssemblyInfo info;
                                  std::visit(overloaded {
                                                 [&](double d) {
                                                     info.code = "ldc2_w " + std::to_string(d) + "\n";
                                                     info.updateDepth(2);
                                                     info.type = AssemblyInfo::Type::DOUBLE;
                                                 },
                                                 [&](const std::string& s) {
                                                     info.code = "ldc " + s + "\n";
                                                     info.updateDepth(2);
                                                     info.type = AssemblyInfo::Type::STRING;
                                                 },
                                                 [&](bool b) {
                                                     info.code = b ? "iconst_1\n" : "iconst_0\n";
                                                     info.updateDepth(2);
                                                     info.type = AssemblyInfo::Type::BOOL;
                                                 },
                                                 [&](auto&) { std::cerr << "Undefined expression" << std::endl; } },
                                      l.literal);
                                  return info;
                              },
                              [&](auto&) -> AssemblyInfo {
                                  throw std::runtime_error("Unsupported expression type");
                              } },
            expr.content);
    }

private:
    template <class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };
    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    bool isTruthy(Expr object)
    {
        if (std::holds_alternative<Literal>(object.content)) {
            Literal l = std::get<Literal>(object.content);
            if (std::holds_alternative<nullptr_t>(l.literal))
                return false;
            if (std::holds_alternative<bool>(l.literal))
                return std::get<bool>(l.literal);
        }
        return true;
    }
};
#endif
