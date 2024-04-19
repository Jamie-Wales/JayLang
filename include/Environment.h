#ifndef INCLUDE_ENVIRONMENT_H_
#define INCLUDE_ENVIRONMENT_H_
#include "AssemblyInfo.h"
#include "Token.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

struct EnvVariable {
    std::string name;
    AssemblyInfo info;
    size_t index;
    size_t scope;
};

// todo make env linked list
class Environment {
public:
    Environment() {};
    size_t varibleCount = 0;
    size_t scope = 0;
    std::unordered_map<std::string, EnvVariable> variables;
    void define(const std::string name, AssemblyInfo info)
    {
        EnvVariable var = { name, info, varibleCount, scope };
        if (variables.find(name) != variables.end()) {
            if (variables.at(name).scope == scope) {
                std::runtime_error error("Cannot redefine " + name);
                throw error;
            }
        }
        variables[name + std::to_string(scope)] = var;
        varibleCount++;
    }

    int assign(std::string name, AssemblyInfo info)
    {
        int scope = this->scope;
        while (scope >= 0) {
            for (auto& [index, value] : variables) {
                if (index == name + std::to_string(scope)) {
                    auto curr = variables.at(name + std::to_string(scope));
                    EnvVariable next { name, info, curr.index, (size_t)scope };
                    variables[name + std::to_string(scope)] = next;
                    return curr.index;
                }
            }
            scope--;
        }
        std::runtime_error error("Cannot assign " + name + " does not exist");
        return -1;
    }

    std::shared_ptr<EnvVariable> get(const std::string name)
    {
        int scope = this->scope;
        while (scope >= 0) {
            for (auto& [index, value] : variables) {
                if (index == name + std::to_string(scope)) {
                    return std::make_shared<EnvVariable>(value);
                }
            }
            scope--;
        }
        return nullptr;
    }

    void clear()
    {
        std::vector<std::string> toRemove;
        size_t scope = this->scope;
        for (auto& [index, value] : variables) {
            if (value.scope == scope) {
                toRemove.push_back(index);
            }
        }

        for (auto& index : toRemove) {
            variables.erase(index);
        }
    }
};

#endif // INCLUDE_INCLUDE_ENVIRONMENT_H_
