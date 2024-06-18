#ifndef INCLUDE_LINKER_H_
#define INCLUDE_LINKER_H_
#include <fstream>

class Linker {
public:
    explicit Linker(const std::string &className) {
        code = ".class public " + className + "\n"
                ".super java/lang/Object\n"
                ".method public static main : ([Ljava/lang/String;)V\n"
                ".code stack 100 locals 10\n";
    }

    void addCode(const std::string &code) {
        this->code += code;
    }

    void writeToFile(const std::string &filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing.");
        }
        file << code << "\n"
                << ".end code\n"
                << ".end method\n"
                << ".end class\n";

        file.close();
    }

private:
    std::string code;
};

#endif
