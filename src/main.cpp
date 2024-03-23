#include "Compiler.h"
#include "Linker.h"
#include "Parser.h"
#include "Scanner.h"
#include "jni.h"
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
JNIEnv* create_vm(JavaVM** jvm)
{
    JNIEnv* env;
    JavaVMInitArgs vm_args;
    JavaVMOption options;
    options.optionString = "-Djava.class.path=.";
    vm_args.version = JNI_VERSION_1_8;
    vm_args.nOptions = 1;
    vm_args.options = &options;
    vm_args.ignoreUnrecognized = 0;

    int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
    if (ret < 0)
        std::cerr << "Unable to Launch JVM\n";
    return env;
}
void runJavaMethod(JNIEnv* env)
{
    jclass clazz = env->FindClass("Example");
    if (clazz == nullptr) {
        std::cerr << "Failed to find the class\n";
        return;
    }

    jmethodID mid = env->GetStaticMethodID(clazz, "main", "([Ljava/lang/String;)V");
    if (mid == nullptr) {
        std::cerr << "Failed to find the static method\n";
        return;
    }
    env->CallStaticVoidMethod(clazz, mid);
}

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
        JavaVM* jni;
        JNIEnv* env = create_vm(&jni);

        std::string compileCommand = "../libs/Krakatau/target/release/krak2 asm --out ./ ./example.j";
        if (system(compileCommand.c_str()) != 0) {
            std::cerr << "Compilation failed.\n";
            return;
        }
        runJavaMethod(env);
        jni->DestroyJavaVM();
        exit(EXIT_SUCCESS);
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
