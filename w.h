#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct Function {
    int startLine;
    int endLine;
};

bool evaluateCondition(const std::string& expr, std::unordered_map<std::string, int>& vars);

void executeCommand(
    const std::vector<std::string>& lines,
    int& i,
    std::unordered_map<std::string, int>& vars,
    std::unordered_map<std::string, std::string>& stringVars,
    std::unordered_map<std::string, Function>& functions);

void preprocessFunctions(
    const std::vector<std::string>& lines,
    std::unordered_map<std::string, Function>& functions);
