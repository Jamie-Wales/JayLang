#ifndef INCLUDE_ENVIRONMENT_H_
#define INCLUDE_ENVIRONMENT_H_
#include "AssemblyInfo.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct EnvVariable {
    std::string name;
    AssemblyInfo info;
    size_t index;
};

class Environment {
public:
    Environment() = default;

    static size_t varibleCount;
    size_t envindex = 0;
    Environment *parent = nullptr;
    Environment *child = nullptr;
    std::unordered_map<std::string, EnvVariable> variables;

    void define(const std::string &name, const AssemblyInfo &info) {
        const EnvVariable var = {name, info, varibleCount};
        if (variables.find(name) != variables.end()) {
            std::runtime_error error("Cannot redefine " + name);
            throw error;
        }
        variables[name] = var;
        varibleCount++;
    }

    Environment *createChild() {
        const auto newEnv = new Environment();
        newEnv->parent = this;
        child = newEnv;
        child->envindex = this->envindex + 1;
        return newEnv;
    }

    int assign(std::string name, AssemblyInfo info) {
        Environment *child = this;
        while (child != nullptr) {
            for (auto &[index, value]: variables) {
                if (index == name) {
                    auto curr = variables.at(name);
                    EnvVariable next{name, info, curr.index};
                    variables[name] = next;
                    return curr.index;
                }
            }
            child = child->parent;
        }

        std::runtime_error error("Cannot assign " + name + " does not exist");
        return -1;
    }

    std::shared_ptr<EnvVariable> get(const std::string &name) {
        auto parent = this;
        while (parent != nullptr) {
            for (auto &[index, value]: variables) {
                if (index == name) {
                    return std::make_shared<EnvVariable>(value);
                }
            }
            parent = parent->parent;
        }
        return nullptr;
    }

    void clear() {
        struct pair {
            Environment *env;
            std::string name;
        };

        std::vector<pair> toRemove;
        Environment *parent = this;
        while (parent != nullptr) {
            for (auto &[index, value]: variables) {
                toRemove.push_back({parent, index});
            }
            parent = parent->parent;
        }

        for (auto &[env, name]: toRemove) {
            env->variables.erase(name);
        }
    }
};

#endif
