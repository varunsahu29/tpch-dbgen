#pragma once
#include <vector>
#include <string>
#include <sstream>
std::vector<std::string> splitLine(const std::string &line, char delimiter= '|') {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}