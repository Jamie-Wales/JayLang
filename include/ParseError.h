#pragma once
#include "Token.h"
#include <stdexcept>
#include <string>

class ParseError final : public std::runtime_error {
public:
    explicit ParseError(const Token &token, const std::string &message)
        : runtime_error(token.toString() + " " + message) {
    }
};