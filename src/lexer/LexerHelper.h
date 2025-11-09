#pragma once
#include <string>
#include <map>

class LexerHelper {
public:
    static const std::map<std::string, std::string> TOKENS;
    static std::vector<std::string> getTokens(std::string& line);
private:
    static std::string isToken(const std::string& value);
};

