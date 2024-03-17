#include "Expression.h"
#include "Parser.h"
#include "Token.h"
#include <Parser.h>
#include <__variant/monostate.h>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#define BUFFERSIZE 2048
void error(int line, std::string message);
static bool ERROR = false;

/*
 *
 *   SCANNER
 *
 *
 */
class Scanner {
private:
    const std::string source;
    std::vector<Token> tokens {};
    size_t start = 0;
    size_t current = 0;
    size_t line = 0;

    std::unordered_map<std::string, TokenType> keywords {
        { "and", TokenType::AND },
        { "class", TokenType::CLASS },
        { "else", TokenType::ELSE },
        { "false", TokenType::FALSE },
        { "for", TokenType::FOR },
        { "func", TokenType::FUNC },
        { "if", TokenType::IF },
        { "nil", TokenType::NIL },
        { "or", TokenType::OR },
        { "lg", TokenType::PRINT },
        { "return", TokenType::RETURN },
        { "super", TokenType::SUPER },
        { "this", TokenType::THIS },
        { "true", TokenType::TRUE },
        { "jj", TokenType::JJ },
        { "while", TokenType::WHILE },
    };
    char peek()
    {
        if (isAtEnd())
            return '\0';

        return source.at(current);
    }

    char peekNext()
    {

        if (current + 1 >= source.length())
            return '\0';

        return source.at(current + 1);
    }

    bool isAtEnd()
    {
        return current >= source.length();
    }

    bool match(char expected)
    {
        if (isAtEnd())
            return false;

        if (source.at(current) != expected)
            return false;

        current++;
        return true;
    }

    static bool isDigit(const char c)
    {
        return c >= '0' && c <= '9';
    }

    static bool isAlpha(const char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    static bool isAlphaNumberic(const char c)
    {
        return isAlpha(c) || isDigit(c);
    }

    void number()
    {
        while (isDigit(peek()))
            advance();

        if (peek() == '.') {
            advance();
            while (isDigit(peek()))
                advance();
        }

        double output = stod(source.substr(start, current));
        auto ptr = std::make_shared<double>(output);
        addToken(TokenType::NUMBER, ptr);
    }

    void string()
    {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n')
                line++;

            advance();
        }

        if (isAtEnd()) {
            error(line, "Unterminated String");
            return;
        }

        advance();
        std::string output = source.substr(start + 1, current - 1);

        auto shared = std::make_shared<std::string>(output);
        addToken(TokenType::STRING, shared);
    }

    void comment()
    {
        while (peek() != '*' && peekNext() != '/' && !isAtEnd()) {
            if (source.at(current) == '\n') {
                line++;
                if (peek() != '*') {
                    error(line, "Malformed comment");
                    return;
                }
            }

            advance();
        }

        if (isAtEnd()) {
            error(line, "Unterminated comment");
            return;
        }

        advance();
        advance();
    }

    char advance()
    {
        return source.at(current++);
    }

    void addToken(TokenType token, std::shared_ptr<void> literal)
    {
        std::string text = source.substr(start, current);
        tokens.emplace_back(token, text, literal, static_cast<int>(line));
    }

    void addToken(TokenType token)
    {
        addToken(token, nullptr);
    }

    void scanToken()
    {
        char c = advance();
        switch (c) {
        case '(':
            addToken(TokenType::LEFT_PAREN);
            break;
        case ')':
            addToken(TokenType::RIGHT_PAREN);
            break;
        case '{':
            addToken(TokenType::LEFT_BRACE);
            break;
        case '}':
            addToken(TokenType::RIGHT_BRACE);
            break;
        case ',':
            addToken(TokenType::COMMA);
            break;
        case '.':
            addToken(TokenType::DOT);
            break;
        case '-':
            addToken(TokenType::MINUS);
            break;
        case '+':
            addToken(TokenType::PLUS);
            break;
        case ';':
            addToken(TokenType::SEMICOLON);
            break;
        case '*':
            addToken(TokenType::STAR);
            break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd())
                    advance();
            } else if (match('*')) {
                comment();
            } else {
                addToken(TokenType::SLASH);
            }

            break;
        case '"':
            string();
            break;
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            break;
        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                while (isAlphaNumberic(peek()))
                    advance();

                TokenType type = TokenType::NONE;
                std::string output = source.substr(start, current);
                try {
                    type = keywords.at(output);
                    addToken(type);
                } catch (const std::out_of_range& e) {
                    addToken(type);
                }

            } else {
                error(line, "Unexpected character.");
            }
            break;
        }
    }

public:
    explicit Scanner(std::string source)
        : source { std::move(source) } {};

    std::vector<Token> scanTokens()
    {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }

        tokens.emplace_back(TokenType::ENDOFFILE, "", nullptr, (int)line);
        return tokens;
    }
};

void runfile(char* path)
{
    if (std::ifstream ifs { path }) {
        std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        Scanner scanner { data };
        std::vector<Token> output = scanner.scanTokens();
        for (const Token& t : output)
            std::cout << t.toString() << std::endl;
        if (ERROR)
            exit(EXIT_FAILURE);
    }
}

void runPrompt()
{
    std::string line = "s";
    while (line != "\0") {
        std::getline(std::cin, line);
        Scanner scan { line };
        std::vector<Token> tokens = scan.scanTokens();
        for (const Token& t : tokens)
            std::cout << t.toString();
    }
}

void report(int line, const std::string& where, const std::string& message)
{
    std::cerr << "🔴 [line " << line << "] Error " << where << ": " << message << std::endl;
}

void error(int line, std::string message)
{
    report(line, "", message);
    ERROR = true;
}

int main(int argc, char* argv[])
{

    std::shared_ptr<double> Pointer = std::make_shared<double>(20.0);
    std::shared_ptr<void> p = std::reinterpret_pointer_cast<void>(Pointer);
    Literal l = { LiteralType::NUMBER, p };

    Unary u { Token { TokenType::MINUS, "-", nullptr, 0 }, std::make_shared<Expr>(ExprType::LITERAL, l) };

    auto strp = std::make_shared<std::string>("example");
    auto pointrstr = std::reinterpret_pointer_cast<void>(strp);
    Literal nl = { LiteralType::STRING, pointrstr };
    auto up = std::make_shared<Expr>(ExprType::UNARY, u);
    auto pnl = std::make_shared<Expr>(ExprType::LITERAL, nl);
    Binary b { up, Token { TokenType::STAR, "*", nullptr, 0 }, pnl };
    auto exprt = ExprType::BINARY;
    Expr expression { exprt, b };
    std::cout << expression << std::endl;
    if (argc > 2) {
        std::cout << "Usage jj [script]" << std::endl;
        exit(EXIT_FAILURE);
    } else if (argc == 2) {
        runfile(argv[1]);
    } else {
        runPrompt();
    }
}
