#include "Compiler.h"
#include "Linker.h"
#include "Parser.h"
#include "Scanner.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const std::string NATIVEIMAGEPATH = "/Users/jamie/Library/Java/JavaVirtualMachines/graalvm-jdk-22.0.1+8.1/Contents/Home/bin/native-image";

void runfile(const std::string& path)
{
    if (!std::filesystem::path(path).has_extension() || std::filesystem::path(path).extension() != ".jay") {
        std::cerr << "Error: Only .jay files are supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ifstream ifs { path };
    if (!ifs) {
        std::cerr << "Failed to open input file: " << path << '\n';
        exit(EXIT_FAILURE);
    }

    std::string baseName = std::filesystem::path(path).stem().string();
    std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    Scanner scanner { data };
    std::vector<Token> output = scanner.scanTokens();

    Parser parser { output };
    auto parse = parser.parse();
    Compiler compiler {};
    AssemblyInfo assem = {};
    Linker linker { baseName };
    for (auto& stmt : parse) {
        linker.addCode(compiler.generateAssembly(*stmt).code);
    }

    compiler.generateLocalVariables(assem, compiler.environment);
    linker.addCode(assem.code);

    std::string outputDir = baseName;
    std::filesystem::create_directories(outputDir + "/src");
    std::filesystem::create_directories(outputDir + "/bin");

    std::string asmFileName = outputDir + "/src/" + baseName + ".j";
    std::string classFileName = outputDir + "/src/" + baseName + ".class";
    std::string executableName = outputDir + "/bin/" + baseName;

    linker.writeToFile(asmFileName);
    std::string compileCommand = "../libs/Krakatau/target/release/krak2 asm --out " + outputDir + "/src " + asmFileName;
    if (system(compileCommand.c_str()) != 0) {
        std::cerr << "Compilation failed.\n";
        return;
    }

    std::string nativeImageCommand = NATIVEIMAGEPATH + " -H:+UnlockExperimentalVMOptions" + " -H:ReflectionConfigurationFiles=" + outputDir + "/META-INF/native-image/reflect-config.json" + " -H:ResourceConfigurationFiles=" + outputDir + "/META-INF/native-image/resource-config.json" + " -H:SerializationConfigurationFiles=" + outputDir + "/META-INF/native-image/serialization-config.json" + " -cp ../jaylib/target/JayLib-0.1.jar:" + outputDir + "/src" + " " + baseName + " -H:Name=" + executableName + " --no-fallback";
    if (system(nativeImageCommand.c_str()) != 0) {
        std::cerr << "Native image generation failed.\n";
        return;
    } else {
        system(executableName.c_str());
    }

    delete compiler.environment;
    exit(EXIT_SUCCESS);
}

int main(const int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: jj [script.jay]" << std::endl;
        exit(EXIT_FAILURE);
    }
    runfile(argv[1]);
}
