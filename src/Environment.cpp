#include "Environment.h"
#include <stdexcept>

size_t Environment::varibleCount = 0;
size_t Environment::envindex = 0;

void Environment::define(const std::string& name, const AssemblyInfo& info)
{
    const EnvVariable var = { name, info, varibleCount };
    if (variables.find(name) != variables.end()) {
        throw std::runtime_error("Cannot redefine " + name);
    }
    variables[name] = var;
    varibleCount++;
}

Environment* Environment::createChild()
{
    auto newEnv = new Environment();
    newEnv->parent = this;
    child = newEnv;
    child->envindex = this->envindex + 1;
    return newEnv;
}

int Environment::assign(const std::string& name, const AssemblyInfo& info)
{
    auto current = this;
    while (current != nullptr) {
        auto it = current->variables.find(name);
        if (it != current->variables.end()) {
            it->second.info = info;
            return static_cast<int>(it->second.index);
        }
        current = current->parent;
    }
    throw std::runtime_error("Cannot assign " + name + ": variable does not exist");
}

std::shared_ptr<EnvVariable> Environment::get(const std::string& name)
{
    auto current = this;
    while (current != nullptr) {
        auto it = current->variables.find(name);
        if (it != current->variables.end()) {
            return std::make_shared<EnvVariable>(it->second);
        }
        current = current->parent;
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
    Environment* current = this;
    while (current != nullptr) {
        for (const auto& [name, _] : current->variables) {
            toRemove.push_back({ current, name });
        }
        current = current->parent;
    }
    for (const auto& [env, name] : toRemove) {
        env->variables.erase(name);
    }
}
