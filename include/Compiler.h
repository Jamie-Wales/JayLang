#ifndef INCLUDE_COMPILER_H
#define INCLUDE_COMPILER_H

#include "Expression.h"
#include <string>
#include <variant>
struct AssemblyInfo {
    std::string code;
    size_t maxStackDepth = 0;
    size_t currentDepth = 0;

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
                              [&](const Binary& b) -> AssemblyInfo {
                                  auto leftInfo = generateAssembly(*b.left);
                                  auto rightInfo = generateAssembly(*b.right);

                                  AssemblyInfo info;
                                  info.code = leftInfo.code + rightInfo.code;
                                  info.code += "dadd\n";
                                  info.updateDepth(2);
                                  info.consume(1);
                                  return info;
                              },
                              [&](const Literal& l) -> AssemblyInfo {
                                  AssemblyInfo info;
                                  std::visit(overloaded {
                                                 [&](double d) {
                                                     info.code = "ldc2_w " + std::to_string(d) + "\n";
                                                     info.updateDepth(2);
                                                 },
                                                 [&](const std::string& s) {
                                                     std::cout << "implement string handling so you can interperate " << s << std::endl;
                                                 },
                                                 [&](bool b) {
                                                     info.code = b ? "iconst_1\n" : "iconst_0\n";
                                                     info.updateDepth(1);
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
};
#endif
