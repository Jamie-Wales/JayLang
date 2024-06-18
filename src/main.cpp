#include "Compiler.h"
#include "Linker.h"
#include "Parser.h"
#include "Scanner.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

const std::string NATIVEIMAGEPATH =
        "/Users/jamie/Library/Java/JavaVirtualMachines/graalvm-jdk-22.0.1+8.1/Contents/Home/bin/native-image";

void runfile(const std::string &path) {
    if (!std::filesystem::path(path).has_extension() || std::filesystem::path(path).extension() != ".jay") {
        std::cerr << "Error: Only .jay files are supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ifstream ifs{path};
    if (!ifs) {
        std::cerr << "Failed to open input file: " << path << '\n';
        exit(EXIT_FAILURE);
    }

    std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    Scanner scanner{data};
    std::vector<Token> output = scanner.scanTokens();
    std::string baseName = std::filesystem::path(path).stem().string();

    Parser parser{output};
    auto parse = parser.parse();
    Compiler compiler{};
    AssemblyInfo assem = {};
    Linker linker{baseName};
    for (auto &stmt: parse) {
        linker.addCode(compiler.generateAssembly(*stmt).code);
    }

    compiler.generateLocalVariables(assem, compiler.environment);
    linker.addCode(assem.code);

    std::string asmFileName = "./" + baseName + ".j";
    std::string classFileName = baseName;

    linker.writeToFile(asmFileName);
    std::string compileCommand = "../libs/Krakatau/target/release/krak2 asm --out ./ " + asmFileName;
    if (system(compileCommand.c_str()) != 0) {
        std::cerr << "Compilation failed.\n";
        return;
    }

    std::string nativeImageCommand = NATIVEIMAGEPATH + " -cp ../jaylib/target/JayLib-0.1.jar:. " + baseName;
    if (system(nativeImageCommand.c_str()) != 0) {
        std::cerr << "Native image generation failed.\n";
        return;
    }

    delete compiler.environment;
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: jj [script.jay]" << std::endl;
        exit(EXIT_FAILURE);
    }
    runfile(argv[1]);
}
