#ifndef INCLUDE_COMPILER_H
#define INCLUDE_COMPILER_H

#include "Expression.h"
#include "Token.h"
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
                                  switch (b.opr.type) {
                                  case TokenType::MINUS:
                                      info.code += leftInfo.code + rightInfo.code + "dsub\n";
                                      break;
                                  case TokenType::SLASH:
                                      info.code += leftInfo.code + rightInfo.code + "ddiv\n";
                                      break;
                                  case TokenType::STAR:
                                      info.code += leftInfo.code + rightInfo.code + "dmul\n";
                                      break;
                                  case TokenType::PLUS:
                                      if (leftInfo.type == AssemblyInfo::Type::DOUBLE && rightInfo.type == AssemblyInfo::Type::DOUBLE) {
                                          info.code += leftInfo.code + rightInfo.code + "dadd\n";
                                          info.type = AssemblyInfo::Type::DOUBLE;

                                      } else if (leftInfo.type == AssemblyInfo::Type::STRING && rightInfo.type == AssemblyInfo::Type::STRING) {
                                          info.code += leftInfo.code + rightInfo.code + "invokevirtual concat(Ljava/lang/String;)Ljava/lang/String;\n";
                                          info.type = AssemblyInfo::Type::STRING;

                                      } else if (leftInfo.type == AssemblyInfo::Type::STRING && rightInfo.type == AssemblyInfo::Type::DOUBLE) {
                                          info.code += leftInfo.code + rightInfo.code + "invokestatic java/lang/String/valueOf(D)Ljava/lang/String;\n" + "invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
                                          info.type = AssemblyInfo::Type::STRING;

                                      } else if (leftInfo.type == AssemblyInfo::Type::DOUBLE && rightInfo.type == AssemblyInfo::Type::STRING) {
                                          info.code += leftInfo.code + "invokestatic java/lang/String/valueOf(D)Ljava/lang/String;\n" + rightInfo.code + "invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
                                          info.type = AssemblyInfo::Type::STRING;
                                      } else {
                                          info.code += leftInfo.code + rightInfo.code + "dadd\n";
                                      }
                                      break;
                                  }
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
