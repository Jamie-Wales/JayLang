#ifndef INCLUDE_INCLUDE_LINKER_H_
#define INCLUDE_INCLUDE_LINKER_H_
#include "Compiler.h"

#include <fstream>
#include <iostream>

void writeToFile(const AssemblyInfo& assemblyInfo, const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing.");
    }
    file << ".class public Example\n"
         << ".super java/lang/Object\n"
         << ".method public static main : ([Ljava/lang/String;)V\n"
         << ".code stack " << 100 << " locals 10\n"
         << "    getstatic java/lang/System/out Ljava/io/PrintStream;\n"
         << "    " << assemblyInfo.code << "\n"
         << "    invokevirtual java/io/PrintStream/println (Ljava/lang/String;)V\n"
         << "    return\n"
         << ".end code\n"
         << ".end method\n"
         << ".end class\n";

    file.close();
}

#endif // INCLUDE_INCLUDE_LINKER_H_
