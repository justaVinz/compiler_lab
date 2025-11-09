#include "LexerHelper.h"
#include <iostream>
#include <ostream>

const std::map<std::string, std::string> LexerHelper::TOKENS = {
    {"KEYWORD", "if"},
    {"KEYWORD", "else"},
    {"KEYWORD", "for"},
    {"KEYWORD", "while"},

    {"SEPERATOR", "("},
    {"SEPERATOR", ")"},
    {"SEPERATOR", "{"},
    {"SEPERATOR", "}"},
    {"SEPERATOR", ";"},
};

std::string LexerHelper::isToken(const std::string& value) {
    for (const auto& [k, val] : TOKENS) {
        if (val == value) return k;
    }
    return "IDENTIFIER";
}

std::vector<std::string> LexerHelper::getTokens(std::string& line) {
    std::string to_check;
    std::vector<std::string> tokens;

    for (char c : line) {
        if (std::isalpha(c)) {
            // collect chars
            to_check.push_back(c);
        } else {
            // check if we found a word
            if (!to_check.empty()) {
                std::string token = LexerHelper::isToken(to_check);
                if (!token.empty())
                    tokens.push_back(token);
                to_check.clear();
            }

            // skip whitespaces
            if (std::isspace(c)) continue;

            // check all other strings
            std::string charStr(1, c);
            std::string token = LexerHelper::isToken(charStr);
            if (!token.empty())
                tokens.push_back(token);
        }
    }

    // check last word
    if (!to_check.empty()) {
        std::string token = LexerHelper::isToken(to_check);
        if (!token.empty())
            tokens.push_back(token);
    }

    return tokens;
}