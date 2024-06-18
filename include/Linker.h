#pragma once

#include <string>

class Linker {
public:
    explicit Linker(const std::string &className);

    void addCode(const std::string &code);
    void writeToFile(const std::string &filename) const;

private:
    std::string code;
};