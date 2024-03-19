#include "Parser.h"
#include "Scanner.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include "Expression.h"
#include <utility>
#define BUFFERSIZE 2048

void runfile(char* path)
{
    if (std::ifstream ifs { path }) {
        std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        Scanner scanner { data };
        std::vector<Token> output = scanner.scanTokens();
        Parser parser { output };
        auto parse = parser.parse();
        std::cout << *parse << std::endl;

        exit(EXIT_FAILURE);
    }
}

void runPrompt()
{
    std::string line = "s";
    while (line != "\0") {
        std::getline(std::cin, line);
        Scanner scan { line };
        std::vector<Token> tokens = scan.scanTokens();
        for (const Token& t : tokens)
            std::cout << t.toString();
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
