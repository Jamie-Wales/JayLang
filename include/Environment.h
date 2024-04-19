#ifndef INCLUDE_ENVIRONMENT_H_
#define INCLUDE_ENVIRONMENT_H_
#include "AssemblyInfo.h"
#include "Token.h"
#include <memory>
#include <stdexcept>
#include <unordered_map>

struct EnvVariable {
    std::string name;
    AssemblyInfo info;
    size_t index;
    size_t scope;
};

class Environment {
public:
    Environment() {};
    size_t varibleCount = 0;
    size_t scope = 0;
    std::unordered_map<std::string, EnvVariable> variables;
    void define(const std::string name, AssemblyInfo info)
    {
        EnvVariable var = { name, info, varibleCount, scope };
        variables[name] = var;
        varibleCount++;
    }

    int assign(std::string name, AssemblyInfo info)
    {
        for (auto& [index, value] : variables) {
            if (index == name) {
                auto curr = variables.at(name);
                EnvVariable next { name, info, curr.index, scope };
                variables[name] = next;
                return curr.index;
            }
        }
        std::runtime_error error("Cannot assign " + name + " does not exist");
        return -1;
    }

    std::shared_ptr<EnvVariable> get(const std::string name)
    {
        for (auto& [index, value] : variables) {
            if (index == name) {
                return std::make_shared<EnvVariable>(value);
            }
        }
        return nullptr;
    }
};

#endif // INCLUDE_INCLUDE_ENVIRONMENT_H_
