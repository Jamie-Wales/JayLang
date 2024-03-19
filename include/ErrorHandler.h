#ifndef INCLUDE_INCLUDE_ERRORHANDLER_H_
#define INCLUDE_INCLUDE_ERRORHANDLER_H_
#include <iostream>

class ErrorHandler {
public:
    bool error = false;

    void report(int line, const std::string& where, const std::string& message)
    {
        std::cerr << "🔴 [line " << line << "] Error " << where << ": " << message << std::endl;
    }

    void handlerError(int line, std::string message)
    {
        report(line, "", message);
        error = true;
    }
};
#endif // INCLUDE_INCLUDE_ERRORHANDLER_H_
