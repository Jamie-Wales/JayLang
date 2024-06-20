#pragma once

#include <fstream>
#include <unordered_map>

class ClassLoader {
public:
    ClassLoader() = default;

    std::string writer;

    void addClass(const std::string &className) {
        classes[className] = "";
    }

    void loadClass(const std::string &loader) {
        writer = classes[loader];
    }


private:
    std::unordered_map<std::string, std::string> classes;
};
