#ifndef INCLUDE_INCLUDE_ERRORHANDLER_H_
#define INCLUDE_INCLUDE_ERRORHANDLER_H_
#include <iostream>

class ErrorHandler {
public:
    bool error = false;

    static void report(const int line, const std::string &where, const std::string &message) {
        std::cerr << "🔴 [line " << line << "] Error " << where << ": " << message << std::endl;
    }

    void handlerError(const int line, const std::string &message) {
        report(line, "", message);
        error = true;
    }
};
#endif // INCLUDE_INCLUDE_ERRORHANDLER_H_
