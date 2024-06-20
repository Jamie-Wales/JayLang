#include "Byteloader.h"

std::string invokevirtual(std::string className, std::string arguments, std::string returnType) {
    if (arguments == "")
        return "invokevirtual " + className + "(" + arguments + ")" + returnType + ";" + "\n";
    else
        return "invokevirtual " + className + "(" + arguments + ";)" + returnType + ";" + "\n";
}
