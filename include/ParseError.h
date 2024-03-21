#ifndef INCLUDE_INCLUDE_PARSEERROR_H_
#define INCLUDE_INCLUDE_PARSEERROR_H_

#include "Token.h"
#include <stdexcept>
#include <string>

class ParseError : public std::runtime_error {
public:
    explicit ParseError(Token& token, const std::string& message)
        : runtime_error(token.toString() + " " + message)
    {
    }
};

#endif // INCLUDE_INCLUDE_PARSERERROR_H_
