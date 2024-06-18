#include "Linker.h"
#include <fstream>
#include <stdexcept>

Linker::Linker(const std::string &className) {
    code = ".class public " + className + "\n"
           ".super java/lang/Object\n"
           ".method public static main : ([Ljava/lang/String;)V\n"
           ".code stack 100 locals 10\n";
}

void Linker::addCode(const std::string &code) {
    this->code += code;
}

void Linker::writeToFile(const std::string &filename) const {
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