#pragma once
#include <string>
#include <unordered_map>

void executeCommand(const std::string& line,
    std::unordered_map<std::string, int>& vars,
    std::unordered_map<std::string, std::string>& stringVars);
