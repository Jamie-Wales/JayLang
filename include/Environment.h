#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "AssemblyInfo.h"

struct EnvVariable {
    std::string name;
    AssemblyInfo info;
    size_t index{};
};

class Environment {
public:
    Environment() = default;

    static size_t varibleCount;
    size_t envindex = 0;
    Environment *parent = nullptr;
    Environment *child = nullptr;
    std::unordered_map<std::string, EnvVariable> variables;

    void define(const std::string &name, const AssemblyInfo &info);
    Environment *createChild();
    int assign(const std::string &name, const AssemblyInfo &info);
    std::shared_ptr<EnvVariable> get(const std::string &name);
    void clear();
};
