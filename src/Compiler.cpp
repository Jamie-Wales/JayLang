#include "Compiler.h"
#include "Environment.h"
#include "Statement.h"
#include <memory>

AssemblyInfo Compiler::generateAssembly(const Statement& stmt)
{
    return std::visit(overloaded {
                          [&](const PrintStatement& ps) {
                              AssemblyInfo info = {};
                              info.code += "getstatic java/lang/System/out Ljava/io/PrintStream;\n";
                              auto exprInfo = generateAssembly(*(ps.expression));
                              info.code += exprInfo.code;
                              info.type = exprInfo.type;
                              if (info.type == AssemblyInfo::Type::DOUBLE) {
                                  info.code += "invokestatic java/lang/String/valueOf(D)Ljava/lang/String;\n";
                              } else if (info.type == AssemblyInfo::Type::BOOL) {
                                  info.code += "invokestatic java/lang/String/valueOf(Z)Ljava/lang/String;\n";
                              }
                              info.code += "invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n";
                              return info;
                          },
                          [&](const ExprStatement& es) {
                              auto info = generateAssembly(*es.expression);
                              return info;
                          },
                          [&](const JJStatement& js) {
                              auto info = generateAssembly(*js.value);
                              environment.define(js.name.getLexeme(), info);
                              auto element = environment.get(js.name.getLexeme());
                              info.code += "astore " + std::to_string(element->index) + "\n";
                              info.updateDepth(+1);
                              return info;
                          },
                          [&](auto&) {
                              throw std::runtime_error("Unsupported statement type");
                          } },
        stmt.content);
};

AssemblyInfo Compiler::generateAssembly(const Expr& expr)
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
                              switch (u.opr.type) {
                              case TokenType::MINUS:
                                  checkNumberOperand(u.opr, info.type);
                                  info.code += "dneg\n";
                                  break;
                              case TokenType::BANG:
                                  isTruthy(*u.value) ? info.code += "iconst_0\n" : info.code += "iconst_1\n";
                                  break;
                              }
                              return info;
                          },
                          [&](const Binary& b) -> AssemblyInfo {
                              auto leftInfo = generateAssembly(*b.left);
                              auto rightInfo = generateAssembly(*b.right);
                              AssemblyInfo info;
                              switch (b.opr.type) {
                              case TokenType::GREATER:
                              case TokenType::GREATER_EQUAL:
                                  checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
                                  info.code += leftInfo.code + rightInfo.code + "dcmpg\n";
                                  break;
                              case TokenType::LESS:
                              case TokenType::LESS_EQUAL:
                                  checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
                                  info.code += leftInfo.code + rightInfo.code + "dcmpl\n";
                                  break;
                              case TokenType::MINUS:
                                  checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
                                  info.code += leftInfo.code + rightInfo.code + "dsub\n";
                                  info.type = AssemblyInfo::Type::DOUBLE;

                                  break;
                              case TokenType::SLASH:
                                  checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
                                  info.code += leftInfo.code + rightInfo.code + "ddiv\n";
                                  info.type = AssemblyInfo::Type::DOUBLE;
                                  break;
                              case TokenType::STAR:
                                  checkNumberOperands(b.opr, leftInfo.type, rightInfo.type);
                                  info.code += leftInfo.code + rightInfo.code + "dmul\n";
                                  info.type = AssemblyInfo::Type::DOUBLE;
                                  break;
                              case TokenType::PLUS:
                                  if (leftInfo.type == AssemblyInfo::Type::DOUBLE && rightInfo.type == AssemblyInfo::Type::DOUBLE) {
                                      info.code += leftInfo.code + rightInfo.code + "dadd\n";
                                      info.type = AssemblyInfo::Type::DOUBLE;
                                  } else if (leftInfo.type == AssemblyInfo::Type::STRING && rightInfo.type == AssemblyInfo::Type::STRING) {
                                      info.code += leftInfo.code + rightInfo.code + "invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
                                      info.type = AssemblyInfo::Type::STRING;

                                  } else if (leftInfo.type == AssemblyInfo::Type::STRING && rightInfo.type == AssemblyInfo::Type::DOUBLE) {
                                      info.code += leftInfo.code + rightInfo.code + "invokestatic java/lang/String/valueOf(D)Ljava/lang/String;\n" + "invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
                                      info.type = AssemblyInfo::Type::STRING;

                                  } else if (leftInfo.type == AssemblyInfo::Type::DOUBLE && rightInfo.type == AssemblyInfo::Type::STRING) {
                                      info.code += leftInfo.code + "invokestatic java/lang/String/valueOf(D)Ljava/lang/String;\n" + rightInfo.code + "invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;\n";
                                      info.type = AssemblyInfo::Type::STRING;
                                  };
                                  break;
                              }
                              return info;
                          },
                          [&](const Variable& v) {
                              AssemblyInfo info;
                              auto element = environment.get(v.name.getLexeme());
                              info = element->info;
                              return info;
                          },
                          [&](auto&) -> AssemblyInfo {
                              throw std::runtime_error("Unsupported expression type");
                          } },
        expr.content);
}

bool Compiler::isTruthy(Expr& object)
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
