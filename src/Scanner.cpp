#include "Scanner.h"
#include "Token.h"
char Scanner::peek()
{
    if (isAtEnd())
        return '\0';

    return source.at(current);
}

char Scanner::peekNext()
{

    if (current + 1 >= source.length())
        return '\0';

    return source.at(current + 1);
}

bool Scanner::isAtEnd()
{
    return current >= source.length();
}

bool Scanner::match(char expected)
{
    if (isAtEnd())
        return false;

    if (source.at(current) != expected)
        return false;

    current++;
    return true;
}

void Scanner::number()
{
    while (isDigit(peek()))
        advance();

    if (peek() == '.') {
        advance();
        while (isDigit(peek()))
            advance();
    }

    double output = stod(source.substr(start, current));
    addToken(TokenType::NUMBER, output);
}

void Scanner::string()
{
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
            line++;

        advance();
    }

    if (isAtEnd()) {
        err.handlerError(line, "Unterminated String");
        return;
    }

    advance();
    std::string output = source.substr(start, current - start);
    addToken(TokenType::STRING, output);
}

void Scanner::comment()
{
    while (peek() != '*' && peekNext() != '/' && !isAtEnd()) {
        if (source.at(current) == '\n') {
            line++;
            if (peek() != '*') {
                err.handlerError(line, "Malformed comment");
                return;
            }
        }

        advance();
    }

    if (isAtEnd()) {
        err.handlerError(line, "Unterminated comment");
        return;
    }

    advance();
    advance();
}

char Scanner::advance()
{
    return source.at(current++);
}

void Scanner::addToken(TokenType token, std::variant<double, std::string, bool, nullptr_t> literal)
{
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(token, text, literal, static_cast<int>(line));
}

void Scanner::addToken(TokenType token)
{
    addToken(token, nullptr);
}

void Scanner::scanToken()
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
    case '?':
        addToken(TokenType::QUESTION_MARK);
        break;
    case ':':
        addToken(TokenType::COLON);
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
            std::string output = source.substr(start, current - start);
            try {
                type = keywords.at(output);
                addToken(type);
            } catch (const std::out_of_range& e) {
                addToken(type);
            }

        } else {
            err.handlerError(line, "Unexpected character.");
        }
        break;
    }
}

std::vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::ENDOFFILE, "", nullptr, (int)line);
    return tokens;
};
