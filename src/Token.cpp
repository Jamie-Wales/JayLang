#include "Token.h"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <variant>
std::string Token::getLexeme() const
{
    return this->lexeme;
}
std::string Token::typeToString() const
{
    switch (type) {
    case TokenType::LEFT_PAREN:
        return "LEFT_PAREN";
    case TokenType::RIGHT_PAREN:
        return "RIGHT_PAREN";
    case TokenType::LEFT_BRACE:
        return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE:
        return "RIGHT_BRACE";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::DOT:
        return "DOT";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::SLASH:
        return "SLASH";
    case TokenType::STAR:
        return "STAR";
    case TokenType::BANG:
        return "BANG";
    case TokenType::BANG_EQUAL:
        return "BANG_EQUAL";
    case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::EQUAL_EQUAL:
        return "EQUAL_EQUAL";
    case TokenType::GREATER:
        return "GREATER";
    case TokenType::GREATER_EQUAL:
        return "GREATER_EQUAL";
    case TokenType::LESS:
        return "LESS";
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::STRING:
        return "STRING";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::AND:
        return "AND";
    case TokenType::CLASS:
        return "CLASS";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::FALSE:
        return "FALSE";
    case TokenType::FUNC:
        return "FUNC";
    case TokenType::FOR:
        return "FOR";
    case TokenType::IF:
        return "IF";
    case TokenType::NIL:
        return "NIL";
    case TokenType::OR:
        return "OR";
    case TokenType::PRINT:
        return "PRINT";
    case TokenType::RETURN:
        return "RETURN";
    case TokenType::SUPER:
        return "SUPER";
    case TokenType::THIS:
        return "THIS";
    case TokenType::TRUE:
        return "TRUE";
    case TokenType::JJ:
        return "JJ";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::ENDOFFILE:
        return "ENDOFFILE";
    case TokenType::QUESTION_MARK:
        return "QUESTION_MARK";
    default:
        return "UNKNOWN";
    }
}

std::string Token::toString() const
{
    std::stringstream ss;
    ss << "TYPE:" << typeToString() << " LEXEME:" << lexeme << " LINE:" << line;
    if (type == TokenType::STRING) {
        const std::string str = std::get<std::string>(literal);
        ss << " LITERAL: " << str;
    } else if (type == TokenType::NUMBER) {
        const double db = std::get<double>(literal);
        ss << "NUMBER:" << db;
    }
    ss << std::endl;
    return ss.str();
}

Token::Token(TokenType type, std::string lexeme, const std::variant<double, std::string, bool, nullptr_t> literal, int line)
    : lexeme(std::move(lexeme))
    , line(line)
    , type(type)
    , literal(std::move(literal)) {};
