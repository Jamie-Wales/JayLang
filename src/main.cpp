#include "Compiler.h"
#include "Linker.h"
#include "Parser.h"
#include "Scanner.h"
#include <fstream>
#include <memory>
#include <string>
#include <vector>

void runfile(char* path)
{
    if (std::ifstream ifs { path }) {
        std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        Scanner scanner { data };
        std::vector<Token> output = scanner.scanTokens();

        Parser parser { output };
        auto parse = parser.parse();
        if (parser.err.error || parse == nullptr)
            return;
        Compiler compiler {};
        writeToFile(compiler.generateAssembly(*parse), "./example.j");
        std::string compileCommand = "../libs/Krakatau/target/release/krak2 asm --out ./ ./example.j";
        if (system(compileCommand.c_str()) != 0) {
            std::cerr << "Compilation failed.\n";
            return;
        }

        std::string runCommand = "java Example";
        if (system(runCommand.c_str()) != 0) {
            std::cerr << "Java program execution failed.\n";
            return;
        }

        exit(EXIT_FAILURE); // Consider revising this to appropriate program logic
    } else {
        std::cerr << "Failed to open input file: " << path << '\n';
    }
    exit(EXIT_FAILURE);
}

void runPrompt()
{
    std::string line = "s";
    while (true) {
        std::getline(std::cin, line);
        if (line == "\0")
            return;
        Scanner scan { line };
        std::vector<Token> tokens = scan.scanTokens();
        Parser parser { tokens };
        auto expr = parser.parse();
        if (parser.err.error && expr == nullptr)
            continue;

        std::cout << *expr << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc > 2) {
        std::cout << "Usage jj [script]" << std::endl;
        exit(EXIT_FAILURE);
    } else if (argc == 2) {
        runfile(argv[1]);
    } else {
        runPrompt();
    }
}
