#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Utils.h"

std::string Utils::readSourceCode(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("File couldn't be opened: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
};