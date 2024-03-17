#ifndef INCLUDE_INCLUDE_TOKEN_H_
#define INCLUDE_INCLUDE_TOKEN_H_
#include <iostream>
#include <sstream>
#include <string>
enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUNC,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    JJ,
    WHILE,

    NONE,
    ENDOFFILE
};

/*
 *
 *  TOKEN
 *
 *
 */
class Token {
private:
    const std::string lexeme;
    const int line;

public:
    const TokenType type;
    const std::shared_ptr<void> literal;
    Token(TokenType type, std::string lexeme, std::shared_ptr<void> literal, int line)
        : lexeme(std::move(lexeme))
        , line(line)
        , type(type)
        , literal(std::move(literal)) {};

    [[nodiscard]] std::string getLexeme() const
    {
        return this->lexeme;
    }
    [[nodiscard]] std::string typeToString() const
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
        default:
            return "UNKNOWN";
        }
    }

    [[nodiscard]] std::string toString() const
    {
        std::stringstream ss;
        ss << "TYPE:" << typeToString() << " LEXEME:" << lexeme << " LINE:" << line;

        // Check and append the literal value if it exists
        if (literal != nullptr) {
            if (type == TokenType::STRING) {
                ss << " LITERAL:\"" << *std::static_pointer_cast<std::string>(literal) << "\"";
            } else if (type == TokenType::NUMBER) {
                ss << " LITERAL:" << *std::static_pointer_cast<double>(literal);
            }
        }

        ss << std::endl;
        return ss.str();
    }
};

#endif // INCLUDE_INCLUDE_TOKEN_H_
