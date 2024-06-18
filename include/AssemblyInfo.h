#pragma once
#include <string>

struct AssemblyInfo {
    std::string code;
    size_t maxStackDepth = 0;
    size_t currentDepth = 0;

    enum class Type {
        DECIMAL,
        STRING,
        BOOL,
        NULL_T,
        VARS,
        VARD,
        VARB
    };

    Type type;

    void updateDepth(const size_t depthNeeded) {
        currentDepth += depthNeeded;
        if (currentDepth > maxStackDepth) {
            maxStackDepth = currentDepth;
        }
    }

    void consume(size_t depthUsed) {
        if (depthUsed <= currentDepth) {
            currentDepth -= depthUsed;
        } else {
            currentDepth = 0;
        }
    }
};