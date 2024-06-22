#include "Environment.h"

size_t Environment::varibleCount = 0;
size_t Environment::envindex = 0;

void Environment::define(const std::string& name, const AssemblyInfo& info)
{
    const EnvVariable var = { name, info, varibleCount };
    if (variables.find(name) != variables.end()) {
        std::runtime_error error("Cannot redefine " + name);
        throw error;
    }
    variables[name] = var;
    varibleCount++;
}

Environment* Environment::createChild()
{
    const auto newEnv = new Environment();
    newEnv->parent = this;
    child = newEnv;
    child->envindex = this->envindex + 1;
    return newEnv;
}

int Environment::assign(const std::string& name, const AssemblyInfo& info)
{
    auto child = this;
    while (child != nullptr) {
        for (auto& [index, value] : variables) {
            if (index == name) {
                auto curr = variables.at(name);
                EnvVariable next { name, info, curr.index };
                variables[name] = next;
                return static_cast<int>(curr.index);
            }
        }
        child = child->parent;
    }

    std::runtime_error error("Cannot assign " + name + " does not exist");
    return -1;
}

std::shared_ptr<EnvVariable> Environment::get(const std::string& name)
{
    auto parent = this;
    while (parent != nullptr) {
        for (auto& [index, value] : variables) {
            if (index == name) {
                return std::make_shared<EnvVariable>(value);
            }
        }
        parent = parent->parent;
    }
    return nullptr;
}

void Environment::clear()
{
    struct pair {
        Environment* env;
        std::string name;
    };

    std::vector<pair> toRemove;
    Environment* parent = this;
    while (parent != nullptr) {
        for (auto& [index, value] : variables) {
            toRemove.push_back({ parent, index });
        }
        parent = parent->parent;
    }

    for (auto& [env, name] : toRemove) {
        env->variables.erase(name);
    }
}
